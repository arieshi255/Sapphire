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

  int count = 0;
  for( auto id : exdData.getIdList< Excel::Fate >() )
  {
    auto fate = exdData.getRow< Excel::Fate >( id );

    if( fate->data().Level != 0 && fate->data().EventRange != 0 )
    {
      auto [ pEventRange, zoneId ] = instanceObjectCache.getEventRangePair( fate->data().EventRange );

      FateData fateData;
      fateData.iconId = fate->data().Icon;
      fateData.layoutId = pEventRange->header.instanceId;
      fateData.handlerId = id; // TODO: generate this properly..

      m_fateZoneMap[ zoneId ].emplace( id, fateData );

      ++count;
    }
  }

  Logger::info( "[FateMgr] Cached {} FATEs", count );
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
    }
  }
}

void Sapphire::World::Manager::FateMgr::onPlayerZoneIn( Entity::Player& player )
{
  m_globalDirector.sendDirectorInit( player );

  auto& server = Common::Service< World::WorldServer >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zone = terriMgr.getZoneByTerritoryTypeId( player.getTerritoryId() );

  if( !zone )
    return;

  for( auto& spawnedFate : m_spawnedFates[ player.getTerritoryId() ] )
  {
    server.queueForPlayer( player.getCharacterId(), { makeActorControlSelf( player.getId(), FateCreateContext, spawnedFate.first ), 
                                                      makeActorControlSelf( player.getId(), SetFateState, spawnedFate.first, Active ) } );
    sendSyncPacket( player, *spawnedFate.second );
  }

  if( m_spawnedFates[ player.getTerritoryId() ].size() > 0 )
    mapMgr.updateFates( zone, m_spawnedFates[ player.getTerritoryId() ], player.getId() );
}

void Sapphire::World::Manager::FateMgr::sendSyncPacket( Entity::Player& player, Fate& fate )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto fateId = fate.getFateId();

  auto fateSyncPacket = makeZonePacket< FFXIVIpcFateSyncLimitTime >( player.getId() );
  fateSyncPacket->data().directorId = fateId;
  fateSyncPacket->data().padding = 0;
  fateSyncPacket->data().startTime = fate.getStartTime();
  fateSyncPacket->data().limitTime = fate.getLimitTime();

  server.queueForPlayer( player.getCharacterId(), fateSyncPacket ); // Should probably be sent to the entire zone too
}

uint8_t Sapphire::World::Manager::FateMgr::getFateCount( uint16_t zoneId )
{
  if( m_spawnedFates.find( zoneId ) == m_spawnedFates.end() )
    return 0;

  return m_spawnedFates[ zoneId ].size();
}

void Sapphire::World::Manager::FateMgr::despawnFate( Fate& fate, FateState state )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zoneId = fate.getZoneId();
  auto zone = terriMgr.getZoneByTerritoryTypeId( zoneId );
  auto fateId = fate.getFateId();

  if( !zone )
    return;

  // Remove fate from the entire zone
  zone->queuePacketForZone( makeActorControlSelf( 0, FateRemoveContext, fateId ) );
  zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, state ) );

  // Remove from spawned fates list
  m_spawnedFates[ zoneId ].erase( fateId );

  mapMgr.updateFates( zone, m_spawnedFates[ zoneId ] );
}

// This should really not depend on a player ref, only a zone id
void Sapphire::World::Manager::FateMgr::spawnFate( Entity::Player& player, uint32_t fateId )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();

  auto zoneId = player.getTerritoryTypeId();
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
    auto spawn = make_Fate( fateId, zoneId, Util::getTimeSeconds(), 300, fate->second ); // TODO: Don't hardcode limit time (might be in exd or gotta map it)

    // Initialize fate
    zone->queuePacketForZone( makeActorControlSelf( 0, FateCreateContext, fateId ) ); 
    zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, Preparing ) );

    // Send sync packet for fate
    sendSyncPacket( player, *spawn );

    // Finalize fate
    zone->queuePacketForZone( makeActorControlSelf( 0, SetFateState, fateId, Active ) );

    // Add to spawned fates list
    m_spawnedFates[ zoneId ].emplace( fateId, std::move( spawn ) );

    // Send map update for fate
    mapMgr.updateFates( zone, m_spawnedFates[ zoneId ] );
    //mapMgr.updateFate( player, spawn );
  }
}