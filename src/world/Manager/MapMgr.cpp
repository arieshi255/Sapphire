#include <Common.h>
#include <Service.h>

#include <datReader/DatCategories/bg/lgb.h>
#include <Exd/ExdData.h>

#include <Event/EventHandler.h>
#include <Event/Director.h>

#include <Network/CommonActorControl.h>
#include <Network/GameConnection.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include <Network/PacketWrappers/ActorControlSelfPacket.h>

#include <Logging/Logger.h>
#include <Util/Util.cpp>

#include <Actor/Player.h>
#include <Inventory/Item.h>

#include "Territory/InstanceObjectCache.h"

#include "MapMgr.h"
#include "FateMgr.h"

#include "Session.h"
#include "WorldServer.h"

using namespace Sapphire::Event;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::WorldPackets::Server;

Sapphire::World::Manager::MapMgr::MapMgr()
{

}

void Sapphire::World::Manager::MapMgr::updateFate( Entity::Player& player, Fate& fate )
{
  std::multimap< uint32_t, EventData, less > newMapData;
  auto fateData = fate.getFateData();

  newMapData.emplace( fate.getFateId(), EventData { fateData.iconId, fateData.layoutId, fateData.handlerId } );

  //sendPackets( player, newMapData, All );
}

void Sapphire::World::Manager::MapMgr::updateFates( TerritoryPtr zone, std::map< uint32_t, FatePtr >& fateZoneData, uint32_t playerId )
{
  std::multimap< uint32_t, EventData, less > newMapData;

  for( auto& fate : fateZoneData )
  {
    auto fateData = fate.second->getFateData();
    newMapData.emplace( fate.first, EventData { fateData.iconId, fateData.layoutId, fateData.handlerId } );
  }

  sendPackets( zone, newMapData, All, playerId );
}

void Sapphire::World::Manager::MapMgr::fillPacket( std::multimap< uint32_t, EventData, less >& mapData, uint32_t* iconIds, uint32_t* layoutIds, uint32_t* handlerIds )
{
  int32_t i = 0;
  for( auto& eventData : mapData )
  {
    iconIds[ i ] = eventData.second.iconId;
    layoutIds[ i ] = eventData.second.layoutId;
    handlerIds[ i ] = eventData.second.handlerId;

    i++;
  }
}

void Sapphire::World::Manager::MapMgr::sendPackets( TerritoryPtr zone, std::multimap< uint32_t, EventData, less >& mapData, UpdateMode updateMode, uint32_t playerId )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  zone->queuePacketForZone( makeActorControlSelf( playerId, Network::ActorControl::BeginMapUpdate, updateMode ) );

  if( mapData.size() <= 2 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker2 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 4 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker4 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 8 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker8 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 16 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker16 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 32 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker32 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 64 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker64 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }
  else if( mapData.size() <= 128 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker128 >( playerId );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    zone->queuePacketForZone( mapUpdatePacket );
  }

  zone->queuePacketForZone( makeActorControlSelf( playerId, Network::ActorControl::FinishMapUpdate ) );
}