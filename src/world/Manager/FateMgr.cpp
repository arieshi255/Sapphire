#include <Common.h>
#include <Service.h>
#include <Logging/Logger.h>

#include <datReader/DatCategories/bg/lgb.h>
#include <Exd/ExdData.h>

#include "Event/Director.h"
#include "Event/EventDefs.h"
#include "Script/ScriptMgr.h"
#include "WorldServer.h"
#include "Manager/PlayerMgr.h"
#include "Manager/TerritoryMgr.h"
#include "Manager/EventMgr.h"
#include "Manager/MapMgr.h"
#include "Manager/RNGMgr.h"

#include "Actor/Player.h"
#include "Actor/EventObject.h"

#include <Network/CommonActorControl.h>
#include <Network/GameConnection.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include <Network/PacketWrappers/ActorControlSelfPacket.h>

#include "Territory/InstanceObjectCache.h"
#include "Territory/Fate.h"

#include "Util/Util.h"

#include "FateMgr.h"

using namespace Sapphire::Common;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::WorldPackets::Server;
using namespace Sapphire::Network::ActorControl;
using namespace Sapphire::World::Manager;

Sapphire::World::Manager::FateMgr::FateMgr()
{
  auto& exdData = Common::Service< Data::ExdData >::ref();
  auto& instanceObjectCache = Common::Service< InstanceObjectCache >::ref();
  auto& RNGMgr = Common::Service< World::Manager::RNGMgr >::ref();

  int count = 0;
  for( auto id : exdData.getIdList< Excel::Fate >() )
  {
    auto fate = exdData.getRow< Excel::Fate >( id );

    if( fate->data().Level != 0 && fate->data().EventRange != 0 && !fate->getString( fate->data().Text.TitleText ).empty() )
    {
      auto [ pEventRange, zoneId ] = instanceObjectCache.getEventRangePair( fate->data().EventRange );

      FateData fateData;
      fateData.iconId = fate->data().Icon;
      fateData.layoutId = pEventRange->header.instanceId;
      fateData.handlerId = id; // TODO: generate this properly..
      fateData.weight = DefaultWeight;
      fateData.rule = static_cast< Common::FateRule >( fate->data().Rule );

      auto timeId = m_fateTimeMap.find( id );

      switch( timeId->second )
      {
        case 2:
          fateData.timeLimit = 1800;
          break;
        case 3:
          fateData.timeLimit = 3600;
          break;
        default:
          fateData.timeLimit = 900;
          break;
      }

      m_fateZoneMap[ zoneId ].emplace( id, fateData );

      ++count;
    }
  }

  Logger::info( "[FateMgr] Cached {} FATEs", count );

  for( auto& fateZone : m_fateZoneMap )
  {
    auto zoneData = m_zoneSpawnMap[ fateZone.first ];
    auto nextSpawn = RNGMgr.getRandGenerator< uint16_t >( SpawnTimeLow, SpawnTimeHigh * 2 ).next();
    m_zoneSpawnMap[ fateZone.first ] = ZoneData{ Util::getTimeSeconds(), nextSpawn };
  }
}

void Sapphire::World::Manager::FateMgr::onInitDirector( Entity::Player& player )
{
  m_globalDirector.sendDirectorVars( player );
}

void Sapphire::World::Manager::FateMgr::onUpdate( uint64_t tick )
{
  auto seconds = tick / 1000;

  for( auto& fateZone : m_spawnedFates )
  {
    for( auto& fate : fateZone.second )
    {
      if( fate.second && seconds >= fate.second->getEndTime() )
      {
        despawnFate( *fate.second, FateState::Failed );
        return;
      }
      else
      {
        fate.second->onUpdate( tick );
      }
    }
  }

  for( auto& fateZone : m_fateZoneMap )
  {
    auto zoneData = m_zoneSpawnMap[ fateZone.first ];

    // If enough time has passed since the last spawn
    if( seconds - zoneData.lastSpawn >= zoneData.spawnTimer )
    {
      // Pick a random FATE
      queueFate( fateZone.first, fateZone.second );
    }
  }
}

void Sapphire::World::Manager::FateMgr::queueFate( uint16_t zoneId, std::map< uint32_t, Common::FateData >& fateZoneData )
{
  auto& RNGMgr = Common::Service< World::Manager::RNGMgr >::ref();
  uint16_t weightSum = 0;
  uint32_t fateId = 0;

  // Add all the weights together
  for( auto& fate : fateZoneData )
    weightSum += fate.second.weight;

  // Find a random number for the next spawn time and for the fate to spawn
  auto random = RNGMgr.getRandGenerator< uint16_t >( 1, weightSum ).next();
  auto nextSpawn = RNGMgr.getRandGenerator< uint16_t >( SpawnTimeLow, SpawnTimeHigh ).next();

  for( auto& fate : fateZoneData )
  {
    // Find a fate by weight priority and make sure it isn't spawned
    if( random < fate.second.weight && m_spawnedFates[ zoneId ].find( fate.first ) == m_spawnedFates[ zoneId ].end() )
    {
      fateId = fate.first;
      break;
    }

    // After each queue, if the weight had been changed last time, reset it (this could be made more advanced, i.e only reset after x amount of queues)
    if( fate.second.weight != DefaultWeight )
      fate.second.weight = DefaultWeight;

    random -= fate.second.weight;
  }

  // Make sure the fate exists before attempting to spawn
  if( fateZoneData.count( fateId ) != 0 )
  {
    spawnFate( zoneId, fateId );
    m_zoneSpawnMap[ zoneId ] = ZoneData{ Util::getTimeSeconds(), nextSpawn };
  }
}

void Sapphire::World::Manager::FateMgr::onPlayerZoneIn( Entity::Player& player )
{
  m_globalDirector.sendDirectorInit( player );

  auto& server = Common::Service< World::WorldServer >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  for( auto& spawnedFate : m_spawnedFates[ player.getTerritoryTypeId() ] )
  {
    server.queueForPlayer( player.getCharacterId(), { makeActorControlSelf( player.getId(), FateCreateContext, spawnedFate.first ), 
                                                      makeActorControlSelf( player.getId(), SetFateState, spawnedFate.first, static_cast< uint32_t >( FateState::Active ) ) } );
    sendSyncPacket( *spawnedFate.second, player.getId() );
  }

  auto zone = terriMgr.getZoneByTerritoryTypeId( player.getTerritoryTypeId() );

  if( !zone )
    return;

  if( m_spawnedFates[ player.getTerritoryTypeId() ].size() > 0 )
    mapMgr.updateFates( zone, m_spawnedFates[ player.getTerritoryTypeId() ], player.getId() );
}

void Sapphire::World::Manager::FateMgr::sendSyncPacket( Fate& fate, uint32_t playerId )
{
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zone = terriMgr.getZoneByTerritoryTypeId( fate.getZoneId() );

  if( !zone )
    return;

  auto fateSyncPacket = makeZonePacket< FFXIVIpcFateSyncLimitTime >( playerId );
  fateSyncPacket->data().directorId = fate.getFateId();
  fateSyncPacket->data().padding = 0;
  fateSyncPacket->data().startTime = fate.getStartTime();
  fateSyncPacket->data().limitTime = fate.getLimitTime();

  zone->queuePacketForZone( fateSyncPacket );
}

std::optional< Sapphire::FatePtr > Sapphire::World::Manager::FateMgr::getFateById( uint32_t fateId )
{
  for( auto& zone : m_spawnedFates )
  {
    auto fate = zone.second.find( fateId );
    if( fate != zone.second.end() )
      return std::optional< Sapphire::FatePtr >( fate->second );
  }

  return std::nullopt;
}

void Sapphire::World::Manager::FateMgr::despawnFate( Fate& fate, FateState state )
{
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zoneId = fate.getZoneId();
  auto zone = terriMgr.getZoneByTerritoryTypeId( zoneId );
  auto fateId = fate.getFateId();

  if( !zone )
    return;

  // Remove fate from the entire zone
  zone->queuePacketForZone( makeActorControlSelf( 0, FateRemoveContext, fateId ) );
  zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, static_cast< uint32_t >( state ) ) );

  // Depending on why it's despawning, adjust the weight accordingly for variance
  if( state == FateState::Failed )
    m_fateZoneMap[ zoneId ][ fateId ].weight *= 1.25f;
  else if( state == FateState::Completed )
    m_fateZoneMap[ zoneId ][ fateId ].weight = 0;

  // Uninitialize fate
  // fate.remove();

  // Remove from spawned fates list
  m_spawnedFates[ zoneId ].erase( fateId );

  mapMgr.updateFates( zone, m_spawnedFates[ zoneId ] );
}

void Sapphire::World::Manager::FateMgr::spawnFate( uint16_t zoneId, uint32_t fateId )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zone = terriMgr.getZoneByTerritoryTypeId( zoneId );

  if( !zone )
    return;

  // Find all fates in zone
  auto& fateDataZone = m_fateZoneMap[ zoneId ];

  auto fate = fateDataZone.find( fateId );
  auto spawnedFates = m_spawnedFates[ zoneId ];

  // Fate found in map but not spawned yet
  if( fate != fateDataZone.end() && spawnedFates.find( fateId ) == spawnedFates.end() )
  {
    auto spawn = make_Fate( fateId, zoneId, Util::getTimeSeconds(), fate->second ); 

    // Initialize fate
    zone->queuePacketForZone( makeActorControlSelf( 0, FateCreateContext, fateId ) ); 
    zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, static_cast< uint32_t >( FateState::Preparing ) ) );

    // Send sync packet for fate
    sendSyncPacket( *spawn );

    // Finalize fate
    zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, static_cast< uint32_t >( FateState::Active ) ) );

    // Let the fate setup itself
    spawn->init();

    // Add to spawned fates list
    m_spawnedFates[ zoneId ].emplace( fateId, std::move( spawn ) );

    // Send map update for fate
    mapMgr.updateFates( zone, m_spawnedFates[ zoneId ] );
  }
}