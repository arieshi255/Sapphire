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

void Sapphire::World::Manager::MapMgr::updateFate( Entity::Player& player, FateMgr::FateData& fateData )
{
  std::multimap< uint32_t, EventData, less > newMapData;

  newMapData.emplace( fateData.handlerId & 0xFFFF, EventData { fateData.iconId, fateData.layoutId, fateData.handlerId } );

  sendPackets( player, newMapData, All );
}

void Sapphire::World::Manager::MapMgr::updateFates( Entity::Player& player, std::map< uint16_t, std::map< uint32_t, FateMgr::FateData > >& fateMapData )
{
  auto& fateDataZone = fateMapData[ player.getTerritoryTypeId() ];
  std::multimap< uint32_t, EventData, less > newMapData;

  for( auto fateData : fateDataZone )
  {
    //if( isFateActive( player, eventData ) ) need to have something like this so it doesn't just add every single fate on the map
    newMapData.emplace( fateData.first, EventData { fateData.second.iconId, fateData.second.layoutId, fateData.second.handlerId } );
  }

  sendPackets( player, newMapData, All );
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

void Sapphire::World::Manager::MapMgr::sendPackets( Entity::Player& player, std::multimap< uint32_t, EventData, less >& mapData, UpdateMode updateMode )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), Network::ActorControl::BeginMapUpdate, updateMode ) );

  if( mapData.size() <= 2 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker2 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 4 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker4 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 8 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker8 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 16 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker16 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 32 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker32 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 64 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker64 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }
  else if( mapData.size() <= 128 )
  {
    auto mapUpdatePacket = makeZonePacket< FFXIVIpcMapMarker128 >( player.getId() );
    mapUpdatePacket->data().numOfMarkers = mapData.size();

    fillPacket( mapData, mapUpdatePacket->data().iconIds, mapUpdatePacket->data().layoutIds, mapUpdatePacket->data().handlerIds );

    server.queueForPlayer( player.getCharacterId(), mapUpdatePacket );
  }

  server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), Network::ActorControl::FinishMapUpdate ) );
}