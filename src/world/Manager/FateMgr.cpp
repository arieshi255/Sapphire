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

void Sapphire::World::Manager::FateMgr::init()
{
}

uint8_t Sapphire::World::Manager::FateMgr::getFateCount( uint16_t zoneId )
{
  if( m_spawnedFates.find( zoneId ) == m_spawnedFates.end() )
    return 0;

  return m_spawnedFates[ zoneId ].size();
}

void Sapphire::World::Manager::FateMgr::spawnFate( Entity::Player& player, uint32_t fateId )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& terriMgr = Common::Service< TerritoryMgr >::ref();
  auto& mapMgr = Common::Service< MapMgr >::ref();

  // Find all fates in zone
  auto& fateDataZone = m_fateZoneMap[ player.getTerritoryTypeId() ];
  auto fate = fateDataZone.find( fateId );

  // Fate found in map
  if( fate != fateDataZone.end() )
  {
    auto zone = terriMgr.getZoneByTerritoryTypeId( player.getTerritoryTypeId() );
    server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), 0x78, fateId ) );
    server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), 0x74, fateId, Preparing ) );

    auto startTime = Util::getTimeSeconds();
    auto limitTime = 300;

    auto fateSyncPacket = makeZonePacket< FFXIVIpcFateSyncLimitTime >( player.getId() );
    fateSyncPacket->data().directorId = fateId;
    fateSyncPacket->data().startTime = startTime;
    fateSyncPacket->data().limitTime = limitTime;

    server.queueForPlayer( player.getCharacterId(), fateSyncPacket );
    server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), 0x74, fateId, Active ) );

    mapMgr.updateFate( player, fate->second );
  }
}