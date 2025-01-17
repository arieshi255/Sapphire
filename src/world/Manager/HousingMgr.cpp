#include "HousingMgr.h"
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>
#include <Exd/ExdData.h>
#include <Network/PacketContainer.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include <Network/PacketWrappers/ActorControlPacket.h>
#include <Network/PacketWrappers/ActorControlSelfPacket.h>
#include <Network/CommonActorControl.h>
#include <Network/GameConnection.h>
#include <Network/Util/PacketUtil.h>

#include <unordered_map>
#include <cstring>
#include <Service.h>
#include <Session.h>

#include "Actor/Player.h"
#include "Actor/EventObject.h"
#include "Event/EventDefs.h"

#include "TerritoryMgr.h"
#include "Territory/HousingZone.h"
#include "Territory/Housing/HousingInteriorTerritory.h"
#include "EventMgr.h"
#include "Territory/Land.h"
#include "WorldServer.h"
#include "Territory/House.h"
#include "InventoryMgr.h"
#include "PlayerMgr.h"
#include "Inventory/HousingItem.h"
#include "Inventory/ItemContainer.h"
#include "Util/UtilMath.h"

using namespace Sapphire;
using namespace Sapphire::World::Manager;
using namespace Sapphire::Network;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::WorldPackets::Server;

HousingMgr::HousingMgr()
{
  m_containerMap[ 0 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems1, Common::InventoryType::HousingInteriorStoreroom1 );
  m_containerMap[ 1 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems2, Common::InventoryType::HousingInteriorStoreroom2 );
  m_containerMap[ 2 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems3, Common::InventoryType::HousingInteriorStoreroom3 );
  m_containerMap[ 3 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems4, Common::InventoryType::HousingInteriorStoreroom4 );
  m_containerMap[ 4 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems5, Common::InventoryType::HousingInteriorStoreroom5 );
  m_containerMap[ 5 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems6, Common::InventoryType::HousingInteriorStoreroom6 );
  m_containerMap[ 6 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems7, Common::InventoryType::HousingInteriorStoreroom7 );
  m_containerMap[ 7 ] = std::make_pair( Common::InventoryType::HousingInteriorPlacedItems8, Common::InventoryType::HousingInteriorStoreroom8 );

  m_internalPlacedItemContainers =
  {
    Common::InventoryType::HousingInteriorPlacedItems1,
    Common::InventoryType::HousingInteriorPlacedItems2,
    Common::InventoryType::HousingInteriorPlacedItems3,
    Common::InventoryType::HousingInteriorPlacedItems4,
    Common::InventoryType::HousingInteriorPlacedItems5,
    Common::InventoryType::HousingInteriorPlacedItems6,
    Common::InventoryType::HousingInteriorPlacedItems7,
    Common::InventoryType::HousingInteriorPlacedItems8,
  };

  m_internalStoreroomContainers =
  {
    Common::InventoryType::HousingInteriorStoreroom1,
    Common::InventoryType::HousingInteriorStoreroom2,
    Common::InventoryType::HousingInteriorStoreroom3,
    Common::InventoryType::HousingInteriorStoreroom4,
    Common::InventoryType::HousingInteriorStoreroom5,
    Common::InventoryType::HousingInteriorStoreroom6,
    Common::InventoryType::HousingInteriorStoreroom7,
    Common::InventoryType::HousingInteriorStoreroom8,
  };
}

HousingMgr::~HousingMgr() = default;

bool HousingMgr::init()
{
  Logger::info( "HousingMgr: Caching housing land init data" );
  //LAND_SEL_ALL

  // 12 wards per territory, 2 territories
  m_landCache.reserve( 12 * 2 );

  initLandCache();

  Logger::debug( "HousingMgr: Checking land counts" );

  size_t houseCount = 0;
  for( auto& landSet : m_landCache )
  {
    auto count = landSet.second.size();

    houseCount += count;

    if( landSet.second.size() != 60 )
    {
      Logger::fatal( "LandSet {0} is missing land entries. Only have {1} land entries.", landSet.first, count );
      return false;
    }
  }

  Logger::info( "HousingMgr: Cached {0} houses", houseCount );

  /////

  if( !loadEstateInventories() )
    return false;

  return true;
}

bool HousingMgr::loadEstateInventories()
{
  Logger::info( "HousingMgr: Loading inventories for estates" );

  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();

  auto stmt = db.getPreparedStatement( Db::LAND_INV_SEL_ALL );
  auto res = db.query( stmt );

  uint32_t itemCount = 0;
  while( res->next() )
  {
    //uint64_t uId, uint32_t catalogId, uint64_t model1, uint64_t model2, bool isHq
    auto ident = res->getUInt64( "LandIdent" );
    auto containerId = res->getUInt16( "ContainerId" );
    auto itemId = res->getUInt64( "ItemId" );
    auto slot = res->getUInt16( "SlotId" );
    auto catalogId = res->getUInt( "catalogId" );
    auto stain = res->getUInt8( "stain" );
    auto characterId = res->getUInt64( "CharacterId" );

    auto item = Inventory::make_HousingItem( itemId, catalogId );
    item->setStain( stain );
    item->setStackSize( 1 );
    // todo: need to set the owner character id on the item

    // set world pos on item if its in an placed item container
    if( isPlacedItemsInventory( static_cast< Common::InventoryType >( containerId ) ) )
    {
      item->setPos( {
        res->getFloat( "PosX" ),
        res->getFloat( "PosY" ),
        res->getFloat( "PosZ" )
      } );

      item->setRot( res->getFloat( "Rotation" ) );
    }

    ContainerIdToContainerMap& estateInv = m_estateInventories[ ident ];

    // check if containerId exists, it always should - if it doesn't, something went wrong
    auto container = estateInv.find( containerId );
    if( container == estateInv.end() )
    {
      Logger::warn( "Skipping item#{0} for ident#{1} - container#{2} doesn't exist for estate.",
                    itemId, ident, containerId );

      continue;
    }

    container->second->setItem( static_cast< uint8_t >( slot ), item );

    itemCount++;
  }

  Logger::debug( "HousingMgr: Loaded {0} inventory items", itemCount );

  return true;
}

void HousingMgr::initLandCache()
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();

  auto stmt = db.getPreparedStatement( Db::LAND_SEL_ALL );
  auto res = db.query( stmt );

  while( res->next() )
  {
    LandCacheEntry entry;

    // land stuff
    entry.m_landSetId = res->getUInt64( "LandSetId" );
    entry.m_landId = static_cast< uint16_t >( res->getUInt( "LandId" ) );

    entry.m_type = static_cast< Common::LandType >( res->getUInt( "Type" ) );
    entry.m_size = static_cast< Common::HouseSize >( res->getUInt8( "Size" ) );
    entry.m_status = static_cast< Common::HouseStatus >( res->getUInt8( "Status" ) );
    entry.m_currentPrice = res->getUInt64( "LandPrice" );
    entry.m_updateTime = res->getUInt64( "UpdateTime" );
    entry.m_ownerId = res->getUInt64( "OwnerId" );

    entry.m_houseId = res->getUInt64( "HouseId" );

    // house stuff
    entry.m_estateWelcome = res->getString( "Welcome" );
    entry.m_estateComment = res->getString( "Comment" );
    entry.m_estateName = res->getString( "HouseName" );
    entry.m_buildTime = res->getUInt64( "BuildTime" );
    entry.m_endorsements = res->getUInt64( "Endorsements" );
    entry.m_hasAetheryte = res->getBoolean( "Aetheryte" );

    m_landCache[ entry.m_landSetId ].push_back( entry );

    uint16_t maxExternalItems = 0;
    uint16_t maxInternalItems = 0;

    // init inventory containers
    switch( entry.m_size )
    {
      case Common::HouseSize::HOUSE_SIZE_S:
        entry.m_maxPlacedExternalItems = 20;
        entry.m_maxPlacedInternalItems = 100;
        break;
      case Common::HouseSize::HOUSE_SIZE_M:
        entry.m_maxPlacedExternalItems = 30;
        entry.m_maxPlacedInternalItems = 150;
        break;
      case Common::HouseSize::HOUSE_SIZE_L:
        entry.m_maxPlacedExternalItems = 40;
        entry.m_maxPlacedInternalItems = 200;
        break;
      default:
        // this should never ever happen, if it does the db is fucked
        Logger::error( "HousingMgr: Plot {0} in landset {1} has an invalid land size, defaulting to cottage.",
                       entry.m_landId, entry.m_landSetId );
        entry.m_maxPlacedExternalItems = 20;
        entry.m_maxPlacedInternalItems = 200;
        break;
    }

    // setup containers
    // todo: this is pretty garbage
    Common::LandIdent ident;
    ident.territoryTypeId = static_cast< int16_t >( entry.m_landSetId >> 16 );
    ident.wardNum = static_cast< int16_t >( entry.m_landSetId & 0xFFFF );
    ident.landId = static_cast< int16_t >( entry.m_landId );
    ident.worldId = 67;

    auto& containers = getEstateInventory( ident );

    auto makeContainer = [ &containers ]( Common::InventoryType type, uint16_t size )
    {
      containers[ type ] = make_ItemContainer( type, size, "houseiteminventory", false, false );
    };

    uint16_t count = 0;
    for( int i = 0; i < 8; ++i )
    {
      if( count > entry.m_maxPlacedInternalItems )
        break;

      auto& pair = m_containerMap[ i ];

      makeContainer( pair.first, 50 );
      makeContainer( pair.second, 50 );

      count += 50;
    }

    // exterior
    makeContainer( Common::InventoryType::HousingExteriorPlacedItems, entry.m_maxPlacedExternalItems );
    makeContainer( Common::InventoryType::HousingExteriorStoreroom, entry.m_maxPlacedExternalItems );

    // fixed containers
    makeContainer( Common::InventoryType::HousingInteriorAppearance, 20 );
    makeContainer( Common::InventoryType::HousingExteriorAppearance, 20 );

  }
}

uint64_t HousingMgr::getNextHouseId()
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto pQR = db.query( "SELECT MAX( HouseId ) FROM house" );

  if( !pQR->next() )
    return 0;

  return pQR->getUInt64( 1 ) + 1;
}

uint32_t HousingMgr::toLandSetId( int16_t territoryTypeId, int16_t wardId ) const
{
  return ( static_cast< uint32_t >( territoryTypeId ) << 16 ) | wardId;
}

LandPtr HousingMgr::getLandByOwnerId( uint64_t id )
{

  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto res = db.query( "SELECT LandSetId, LandId FROM land WHERE OwnerId = " + std::to_string( id ) );

  if( !res->next() )
    return nullptr;

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( res->getUInt( 1 ) );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );
  if( !hZone )
    return nullptr;

  return hZone->getLand( static_cast< uint8_t >( res->getUInt( 2 ) ) );
}

void HousingMgr::sendLandSignOwned( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& playerMgr = Common::Service< World::Manager::PlayerMgr >::ref();
  player.setActiveLand( static_cast< uint8_t >( ident.landId ), static_cast< uint8_t >( ident.wardNum ) );

  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land )
    return;

  auto landInfoSignPacket = makeZonePacket< FFXIVIpcHousingProfile >( player.getId() );
  landInfoSignPacket->data().Size = land->getSize();
  landInfoSignPacket->data().Welcome = static_cast< uint8_t >( land->getLandType() );
  landInfoSignPacket->data().LandId = ident;
  //landInfoSignPacket->data().houseIconAdd = land->getSharing();
  landInfoSignPacket->data().OwnerId = player.getCharacterId(); // todo: should be real owner contentId, not player.contentId()

  if( auto house = land->getHouse() )
  {
    std::strcpy( landInfoSignPacket->data().Name, house->getHouseName().c_str() );
    std::strcpy( landInfoSignPacket->data().Greeting, house->getHouseGreeting().c_str() );
  }

  uint64_t characterId = land->getOwnerId();

  std::string playerName = playerMgr.getPlayerNameFromDb( characterId );

  memcpy( &landInfoSignPacket->data().OwnerName, playerName.c_str(), playerName.size() );

  server.queueForPlayer( player.getCharacterId(), landInfoSignPacket );
}

void HousingMgr::sendLandSignFree( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  player.setActiveLand( static_cast< uint8_t >( ident.landId ), static_cast< uint8_t >( ident.wardNum ) );

  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  auto plotPricePacket = makeZonePacket< FFXIVIpcHousingAuction >( player.getId() );
  plotPricePacket->data().Price = land->getCurrentPrice();
  plotPricePacket->data().Timer = land->getDevaluationTime();

  server.queueForPlayer( player.getCharacterId(), plotPricePacket );
}

LandPurchaseResult HousingMgr::purchaseLand( Entity::Player& player, HousingZone& zone, uint16_t plot, uint8_t state )
{

  auto plotPrice = zone.getLand( plot )->getCurrentPrice();
  auto gilAvailable = player.getCurrency( Common::CurrencyType::Gil );
  auto pLand = zone.getLand( plot );

  if( !pLand )
    return LandPurchaseResult::ERR_INTERNAL;

  if( pLand->getStatus() != Common::HouseStatus::ForSale )
    return LandPurchaseResult::ERR_NOT_AVAILABLE;

  if( gilAvailable < plotPrice )
    return LandPurchaseResult::ERR_NOT_ENOUGH_GIL;


  switch( static_cast< LandPurchaseMode >( state ) )
  {
    case LandPurchaseMode::FC:
      PlayerMgr::sendDebug( player, "Free company house purchase aren't supported at this time." );
      return LandPurchaseResult::ERR_INTERNAL;
    case LandPurchaseMode::PRIVATE:
    {

      auto pOldLand = getLandByOwnerId( player.getCharacterId() );

      if( pOldLand )
        return LandPurchaseResult::ERR_NO_MORE_LANDS_FOR_CHAR;

      player.removeCurrency( Common::CurrencyType::Gil, plotPrice );
      pLand->setOwnerId( player.getCharacterId() );
      pLand->setStatus( Common::HouseStatus::Sold );
      pLand->setLandType( Common::LandType::Private );

      player.setLandFlags( Common::LandFlagsSlot::Private, 0, pLand->getLandIdent() );

      sendLandFlagsSlot( player, Common::LandFlagsSlot::Private );

      //pLand->setLandName( "Private Estate" + std::to_string( pHousing->getWardNum() ) + "-" + std::to_string( plot )   );
      pLand->updateLandDb();

      zone.sendLandUpdate( plot );
      return LandPurchaseResult::SUCCESS;
    }

    default:
      return LandPurchaseResult::ERR_INTERNAL;
  }

}

bool HousingMgr::relinquishLand( Entity::Player& player, HousingZone& zone, uint8_t plot )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  // TODO: Fix "permissions" being sent incorrectly
  // TODO: Add checks for land state before relinquishing

  auto pLand = zone.getLand( plot );
  auto plotMaxPrice = pLand->getCurrentPrice();

  // can't relinquish when you are not the owner
  // TODO: actually use permissions here for FC houses
  if( !pLand || !hasPermission( player, *pLand, 0 ) )
  {
    Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::LogMsg, 3304 );
    return false;
  }

  // unable to relinquish if there is a house built
  // TODO: additionally check for yard items
  if( pLand->getHouse() )
  {
    Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::LogMsg, 3315 );
    return false;
  }

  pLand->setCurrentPrice( pLand->getMaxPrice() );
  pLand->setOwnerId( 0 );
  pLand->setStatus( Common::HouseStatus::ForSale );
  pLand->setLandType( Common::LandType::none );
  pLand->updateLandDb();

  Common::LandIdent ident { 0xFF, 0xFF, 0xFF, 0xFF };

  player.setLandFlags( Common::LandFlagsSlot::Private, 0, ident );

  sendLandFlagsSlot( player, Common::LandFlagsSlot::Private );

  Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::LogMsg, 3351, 0x1AA, pLand->getLandIdent().wardNum + 1, plot + 1 );
  zone.sendLandUpdate( plot );
  return true;
}

void HousingMgr::sendWardLandInfo( Entity::Player& player, uint8_t wardId, uint16_t territoryTypeId )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto landSetId = toLandSetId( territoryTypeId, wardId );

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto wardInfoPacket = makeZonePacket< FFXIVIpcHousingProfileList >( player.getId() );
  wardInfoPacket->data().LandSetId.wardNum = wardId;
  wardInfoPacket->data().LandSetId.territoryTypeId = static_cast< int16_t >( territoryTypeId );

  // todo: properly get worldId
  wardInfoPacket->data().LandSetId.worldId = 67;

  for( int i = 0; i < 30; i++ )
  {
    auto land = hZone->getLand( i );
    assert( land );

    auto& entry = wardInfoPacket->data().ProfileList[ i ];

    // retail always sends the house price in this packet, even after the house has been Sold
    // so I guess we do the same
    entry.price = land->getCurrentPrice();

    if( land->getStatus() == Common::HouseStatus::ForSale )
      continue;

    if( auto house = land->getHouse() )
    {
      if( !house->getHouseGreeting().empty() )
        entry.status |= Common::WardlandFlags::HasEstateGreeting;
    }

    switch( land->getLandType() )
    {
      case Common::LandType::FreeCompany:
        entry.status |= Common::WardlandFlags::IsEstateOwned | Common::WardlandFlags::IsFreeCompanyEstate;

        // todo: send FC name

        break;

      case Common::LandType::Private:
        entry.status |= Common::WardlandFlags::IsEstateOwned;
        /* //Disabled. No more name in Info
        auto owner = land->getOwnerId();
        auto playerName = server.getPlayerNameFromDb( static_cast< uint32_t >( owner ) );
        memcpy( &entry.fcTag, playerName.c_str(), playerName.size() );
        */
        break;
    }

    // todo: check we have an estate message and set the flag
    // todo: check if estate allows public entry
  }

  server.queueForPlayer( player.getCharacterId(), wardInfoPacket );
}

void HousingMgr::sendEstateGreeting( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land )
    return;

  auto house = land->getHouse();
  if( !house )
    return;

  auto greetingPacket = makeZonePacket< FFXIVIpcHousingGreeting >( player.getId() );

  greetingPacket->data().LandId = ident;

  auto greeting = house->getHouseGreeting();
  memcpy( &greetingPacket->data().Greeting, greeting.c_str(), greeting.size() );

  server.queueForPlayer( player.getCharacterId(), greetingPacket );
}

bool HousingMgr::initHouseModels( Entity::Player& player, LandPtr land, uint32_t presetCatalogId )
{
  auto house = land->getHouse();
  assert( house );

  // add containers to inv collection
  auto& houseInventory = getEstateInventory( house->getLandIdent() );
  houseInventory[ Common::InventoryType::HousingInteriorAppearance ];// = intContainer;
  houseInventory[ Common::InventoryType::HousingExteriorAppearance ];// = extContainer;

  auto& exdData = Common::Service< Sapphire::Data::ExdData >::ref();
  auto preset = exdData.getRow< Excel::HousingPreset >( getItemAdditionalData( presetCatalogId ) );
  if( !preset )
    return false;

  // remove preset item
  Inventory::InventoryContainerPair foundItem;
  if( !player.findFirstItemWithId( presetCatalogId, foundItem ) )
    return false;

  auto item = getHousingItemFromPlayer( player, foundItem.first, foundItem.second );
  if( !item )
    return false;

  // move preset item into ext appearance container
  houseInventory[ Common::InventoryType::HousingExteriorAppearance ]->setItem( Common::HouseExteriorSlot::HousePermit, item );

  // high iq shit
  auto invMap = std::map< uint16_t, std::map< uint32_t, int32_t > >
  {
    // external
    {
      Common::InventoryType::HousingExteriorAppearance,
      {
        { Common::HouseExteriorSlot::ExteriorRoof, preset->data().Roof },
        { Common::HouseExteriorSlot::ExteriorWall, preset->data().Wall },
        { Common::HouseExteriorSlot::ExteriorWindow, preset->data().Window },
        { Common::HouseExteriorSlot::ExteriorDoor, preset->data().Door }
      }
    },

    // internal
    {
      Common::InventoryType::HousingInteriorAppearance,
      {
        // lobby/middle floor
        { Common::HouseInteriorSlot::InteriorWall, preset->data().Interior[ 0 ] },
        { Common::HouseInteriorSlot::InteriorFloor, preset->data().Interior[ 1 ] },
        { Common::HouseInteriorSlot::InteriorLight, preset->data().Interior[ 2 ] },

        // attic
        { Common::HouseInteriorSlot::InteriorWall_Attic, preset->data().Interior[ 3 ] },
        { Common::HouseInteriorSlot::InteriorFloor_Attic, preset->data().Interior[ 4 ] },
        { Common::HouseInteriorSlot::InteriorLight_Attic, preset->data().Interior[ 5 ] },

        // basement
        { Common::HouseInteriorSlot::InteriorWall_Basement, preset->data().Interior[ 6 ] },
        { Common::HouseInteriorSlot::InteriorFloor_Basement, preset->data().Interior[ 7 ] },
        { Common::HouseInteriorSlot::InteriorLight_Basement, preset->data().Interior[ 8 ] },
      }
    }
  };

  auto& invMgr = Common::Service< InventoryMgr >::ref();

  // create and link items
  for( auto& destContainer : invMap )
  {
    auto& container = houseInventory[ destContainer.first ];

    for( auto& itemIt : destContainer.second )
    {
      // small houses attic is just 0, ignore them
      if( itemIt.second == 0 )
        continue;

      auto pItem = invMgr.createItem( player, static_cast< uint32_t >( itemIt.second ) );

      container->setItem( static_cast< uint8_t >( itemIt.first ), pItem );
    }

    invMgr.saveHousingContainer( land->getLandIdent(), container );
  }

  // lift off
  updateHouseModels( house );

  return true;
}

void HousingMgr::createHouse( Sapphire::HousePtr house ) const
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();

  auto stmt = db.getPreparedStatement( Db::HOUSING_HOUSE_INS );
  // LandSetId, HouseId, HouseName

  stmt->setUInt( 1, house->getLandSetId() );
  stmt->setUInt( 2, house->getId() );
  stmt->setString( 3, house->getHouseName() );

  db.execute( stmt );
}

void HousingMgr::deleteHouse( Sapphire::HousePtr house ) const
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto stmt = db.getPreparedStatement( Db::HOUSING_HOUSE_DEL );
  stmt->setUInt( 1, house->getId() );
  db.execute( stmt );
}

void HousingMgr::buildPresetEstate( Entity::Player& player, HousingZone& zone, uint8_t plotNum, uint32_t presetCatalogId )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto pLand = zone.getLand( plotNum );
  if( !pLand || !hasPermission( player, *pLand, 0 ) )
    return;

  // create house
  auto ident = pLand->getLandIdent();
  auto house = make_House( getNextHouseId(), pLand->getLandSetId(), ident, "Estate #" + std::to_string( ident.landId + 1 ), "" );

  pLand->setHouse( house );

  // create inventory items
  if( !initHouseModels( player, pLand, presetCatalogId ) )
  {
    pLand->setHouse( nullptr );
    Logger::error( "House failed building because of initHouseModels failed" );
    return;
  }

  auto& eventMgr = Common::Service< World::Manager::EventMgr >::ref();

  createHouse( house );

  pLand->setStatus( Common::HouseStatus::PrivateEstate );
  pLand->setLandType( Common::LandType::Private );
  zone.sendLandUpdate( plotNum );

  Network::Util::Packet::sendActorControl( player, player.getId(), ActorControl::BuildPresetResponse, plotNum );

  pLand->updateLandDb();

  // start house built event
  // CmnDefHousingBuildHouse_00149
  eventMgr.eventStart( player, player.getId(), 0x000B0095, Event::EventHandler::EventType::Housing, 1, 1 );
  eventMgr.playScene( player, 0x000B0095, 0, static_cast< uint32_t >( SET_BASE | HIDE_HOTBAR ), { 1, plotNum } );

  player.setLandFlags( Common::LandFlagsSlot::Private, Common::HOUSING_LAND_STATUS::HOUSING_LAND_STATUS_BUILDHOUSE, ident );
  sendLandFlagsSlot( player, Common::LandFlagsSlot::Private );

  zone.registerEstateEntranceEObj( plotNum );
}

void HousingMgr::requestEstateRename( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );

  auto house = land->getHouse();
  if( !house )
    return;

  auto landRenamePacket = makeZonePacket< FFXIVIpcHousingHouseName >( player.getId() );

  landRenamePacket->data().LandId = ident;
  memcpy( &landRenamePacket->data().Name, house->getHouseName().c_str(), 20 );

  server.queueForPlayer( player.getCharacterId(), landRenamePacket );
}

void HousingMgr::requestEstateEditGreeting( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land )
    return;

  auto house = land->getHouse();
  if( !house )
    return;

  auto estateGreetingPacket = makeZonePacket< FFXIVIpcHousingGreeting >( player.getId() );

  estateGreetingPacket->data().LandId = ident;
  memcpy( &estateGreetingPacket->data().Greeting, house->getHouseGreeting().c_str(), sizeof( estateGreetingPacket->data().Greeting ) );

  server.queueForPlayer( player.getCharacterId(), estateGreetingPacket );
}

void HousingMgr::updateEstateGreeting( Entity::Player& player, const Common::LandIdent ident, const std::string& greeting )
{
  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land || !hasPermission( player, *land, 0 ) )
    return;

  auto house = land->getHouse();
  if( !house )
    return;

  house->setHouseGreeting( greeting );

  // Greeting updated.
  PlayerMgr::sendLogMessage( player, 3381 );
}

void HousingMgr::requestEstateEditGuestAccess( Entity::Player& player, const Common::LandIdent ident )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  if( !hZone )
    return;

  auto land = hZone->getLand( ident.landId );
  if( !land || !hasPermission( player, *land, 0 ) )
    return;

  auto packet = makeZonePacket< FFXIVIpcHousingWelcome >( player.getId() );
  packet->data().LandId = ident;
  packet->data().Welcome = land->getSharing();

  server.queueForPlayer( player.getCharacterId(), packet );
}

Common::LandIdent HousingMgr::clientTriggerParamsToLandIdent( uint32_t param11, uint32_t param12, bool use16bits ) const
{
  Common::LandIdent ident{};
  ident.worldId = static_cast< int16_t >( param11 >> 16 );
  ident.territoryTypeId = static_cast< int16_t >( param11 & 0xFFFF );

  if( use16bits )
  {
    ident.wardNum = static_cast< int16_t >( param12 >> 16 );
    ident.landId = static_cast< int16_t >( param12 & 0xFFFF );
  }
  else
  {
    ident.wardNum = (param12 >> 8) & 0xFF;
    ident.landId = param12 & 0xFF;
  }

  return ident;
}

void HousingMgr::sendEstateInventory( Entity::Player& player, uint16_t inventoryType, uint8_t plotNum )
{
  Sapphire::LandPtr targetLand;
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  // plotNum will be 255 in the event that it's an internal zone
  // and we have to switch up our way of getting the LandPtr
  if( plotNum == 255 )
  {
    auto internalZone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
    if( !internalZone )
      return;

    auto ident = internalZone->getLandIdent();

    auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );
    auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
    auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

    if( !hZone )
      return;

    targetLand = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  }
  else
  {
    auto zone = std::dynamic_pointer_cast< HousingZone >( pZone );
    if( !zone )
      return;

    targetLand = zone->getLand( plotNum );
  }

  if( !targetLand || !hasPermission( player, *targetLand, 0 ) )
    return;

  auto& containers = getEstateInventory( targetLand->getLandIdent() );
  auto it = containers.find( inventoryType );
  if( it == containers.end() )
    return;

  auto& invMgr = Common::Service< Manager::InventoryMgr >::ref();
  invMgr.sendInventoryContainer( player, it->second );
}

const HousingMgr::LandSetLandCacheMap& HousingMgr::getLandCacheMap()
{
  return m_landCache;
}

HousingMgr::LandIdentToInventoryContainerMap& HousingMgr::getEstateInventories()
{
  return m_estateInventories;
}

HousingMgr::ContainerIdToContainerMap& HousingMgr::getEstateInventory( uint64_t ident )
{
  return m_estateInventories[ ident ];
}

HousingMgr::ContainerIdToContainerMap& HousingMgr::getEstateInventory( Sapphire::Common::LandIdent ident )
{
  auto u64ident = *reinterpret_cast< uint64_t* >( &ident );

  return getEstateInventory( u64ident );
}

void HousingMgr::updateHouseModels( HousePtr house )
{
  assert( house );

  auto& containers = getEstateInventory( house->getLandIdent() );

  auto extContainer = containers.find( static_cast< uint16_t >( Common::InventoryType::HousingExteriorAppearance ) );
  if( extContainer != containers.end() )
  {
    for( auto& item : extContainer->second->getItemMap() )
    {
      // in the Slot array, the first slot is actually the permit
      // but the models array starts from the 2nd entry of the enum
      // so we skip the first one, and then any subsequent entries is slotid - 1

      auto slotId = item.first - 1;
      if( slotId < 0 )
        continue;

      house->setExteriorModel( static_cast< Common::HouseExteriorSlot >( slotId ),
                               getItemAdditionalData( item.second->getId() ), item.second->getStain() );
    }
  }
  else
  {
    Logger::error( "Plot {0} has an invalid inventory configuration for outdoor appearance.", house->getLandIdent().landId );
  }

  auto intContainer = containers.find( static_cast< uint16_t >( Common::InventoryType::HousingInteriorAppearance ) );
  if( intContainer != containers.end() )
  {
    for( auto& item : intContainer->second->getItemMap() )
    {
      house->setInteriorModel( static_cast< Common::HouseInteriorSlot >( item.first ), getItemAdditionalData( item.second->getId() ) );
    }
  }
  else
  {
    Logger::error( "Plot {0} has an invalid inventory configuration for indoor appearance.", house->getLandIdent().landId );
  }
}

uint32_t HousingMgr::getItemAdditionalData( uint32_t catalogId )
{
  auto& pExdData = Common::Service< Data::ExdData >::ref();
  auto info = pExdData.getRow< Excel::Item >( catalogId );
  return info->data().CategoryArg;
}

bool HousingMgr::isPlacedItemsInventory( Sapphire::Common::InventoryType type )
{
  return type == Common::InventoryType::HousingExteriorPlacedItems  ||
         type == Common::InventoryType::HousingInteriorPlacedItems1 ||
         type == Common::InventoryType::HousingInteriorPlacedItems2 ||
         type == Common::InventoryType::HousingInteriorPlacedItems3 ||
         type == Common::InventoryType::HousingInteriorPlacedItems4 ||
         type == Common::InventoryType::HousingInteriorPlacedItems5 ||
         type == Common::InventoryType::HousingInteriorPlacedItems6;
}

void HousingMgr::reqPlaceHousingItem( Entity::Player& player, uint16_t landId, uint16_t containerId, uint8_t slotId,
                                      Common::FFXIVARR_POSITION3 pos, float rotation )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  // retail process is:
  //  - unlink item from current container
  //  - add it to destination container
  //  - resend container
  //  - send spawn packet
  //  - send actrl 3f3, all params are 0

  LandPtr land;
  bool isOutside = false;

  // inside housing territory
  if( auto zone = std::dynamic_pointer_cast< HousingZone >( pZone ) )
  {
    land = zone->getLand( static_cast< uint8_t >( landId ) );

    isOutside = true;
  }
  // otherwise, inside a house. landId is 0 when inside a plot
  else if( auto zone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone ) )
  {
    // todo: this whole process is retarded and needs to be fixed
    // perhaps maintain a list of estates by ident inside housingmgr?
    auto ident = zone->getLandIdent();
    auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

    auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
    auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

    land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  }
  // wtf?
  else
    return;

  if( !land || !hasPermission( player, *land, 0 ) )
    return;

  // todo: check item position and make sure it's not outside the plot
  // anecdotal evidence on reddit seems to imply retail uses a radius based check

  // unlink item
  Inventory::HousingItemPtr item;

  if( containerId == Common::InventoryType::Bag0 ||
      containerId == Common::InventoryType::Bag1 ||
      containerId == Common::InventoryType::Bag2 ||
      containerId == Common::InventoryType::Bag3 )
  {
    item = getHousingItemFromPlayer( player, static_cast< Common::InventoryType >( containerId ), slotId );
    if( !item )
      return;

    // set params
    item->setPos( pos );
    item->setRot( rotation );
  }
  else
  {
    PlayerMgr::sendUrgent( player, "The inventory you are using to place an item is not supported." );
    return;
  }

  auto ident = land->getLandIdent();

  bool status = false;

  if( isOutside )
    status = placeExternalItem( player, item, ident );
  else
    status = placeInteriorItem( player, item );

  if( status )
    Network::Util::Packet::sendActorControlSelf( player, player.getId(), 0x3f3 );
  else
    PlayerMgr::sendUrgent( player, "An internal error occurred when placing the item." );
}

void HousingMgr::reqPlaceItemInStore( Entity::Player& player, uint16_t landId, uint16_t containerId, uint8_t slotId )
{
  LandPtr land;
  bool isOutside = false;

  auto& teriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  if( auto zone = std::dynamic_pointer_cast< HousingZone >( pZone ) )
  {
    land = zone->getLand( static_cast< uint8_t >( landId ) );
    isOutside = true;
  }
  else if( auto zone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone ) )
  {
    // todo: this whole process is retarded and needs to be fixed
    // perhaps maintain a list of estates by ident inside housingmgr?
    auto ident = zone->getLandIdent();
    auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

    auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
    auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

    land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  }

  if( !hasPermission( player, *land, 0 ) )
    return;

  auto& invMgr = Common::Service< InventoryMgr >::ref();
  auto ident = land->getLandIdent();
  auto& containers = getEstateInventory( ident );

  if( isOutside )
  {
    auto& container = containers[ Common::InventoryType::HousingExteriorStoreroom ];

    auto freeSlot = container->getFreeSlot();
    if( freeSlot == -1 )
      return;

    auto item = getHousingItemFromPlayer( player, static_cast< Common::InventoryType >( containerId ), slotId );
    if( !item )
      return;

    container->setItem( static_cast< uint8_t >( freeSlot ), item );
    invMgr.sendInventoryContainer( player, container );
    invMgr.saveHousingContainer( ident, container );
  }
  else
  {
    for( auto houseContainer : m_internalStoreroomContainers )
    {
      auto it = containers.find( houseContainer );
      if( it == containers.end() )
        continue;

      auto container = it->second;
      auto freeSlot = container->getFreeSlot();
      if( freeSlot == -1 )
      {
        continue;
      }

      auto item = getHousingItemFromPlayer( player, static_cast< Common::InventoryType >( containerId ), slotId );
      if( !item )
        return;

      container->setItem( static_cast< uint8_t >( freeSlot ), item );
      invMgr.sendInventoryContainer( player, container );
      invMgr.saveHousingContainer( ident, container );
    }
  }
}

bool HousingMgr::placeExternalItem( Entity::Player& player, Inventory::HousingItemPtr item, Common::LandIdent ident )
{
  auto& invMgr = Common::Service< InventoryMgr >::ref();

  auto& container = getEstateInventory( ident )[ Common::InventoryType::HousingExteriorPlacedItems ];

  auto freeSlot = container->getFreeSlot();

  // todo: what happens when this fails? at the moment the player will just lose the item
  if( freeSlot == -1 )
    return false;

  // add item to inv
  container->setItem( static_cast< uint8_t >( freeSlot ), item );

  // we need to save the item again as removing it from the container on the player will remove it from charaglobalitem
  // todo: this needs to be handled a bit better as it might be possible to overwrite another item that is created in the meantime
  invMgr.saveItem( player, item );

  invMgr.sendInventoryContainer( player, container );
  invMgr.saveHousingContainer( ident, container );
  invMgr.updateHousingItemPosition( item );

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  // add to zone and spawn
  auto zone = std::dynamic_pointer_cast< HousingZone >( pZone );
  assert( zone );

  zone->spawnYardObject( ident.landId, static_cast< uint16_t >( freeSlot ), *item );

  return true;
}

bool HousingMgr::placeInteriorItem( Entity::Player& player, Inventory::HousingItemPtr item )
{
  auto& invMgr = Common::Service< InventoryMgr >::ref();
  auto& teriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto zone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
  assert( zone );

  auto ident = zone->getLandIdent();

  auto& containers = getEstateInventory( ident );

  // find first free container
  uint8_t containerIdx = 0;
  for( auto containerId : m_internalPlacedItemContainers )
  {
    auto it = containers.find( containerId );
    if( it == containers.end() )
      continue;

    auto container = it->second;
    auto freeSlot = container->getFreeSlot();
    if( freeSlot == -1 )
    {
      containerIdx++;
      continue;
    }

    // have a free slot
    container->setItem( static_cast< uint8_t >( freeSlot ), item );

    // resend container
    invMgr.sendInventoryContainer( player, container );
    invMgr.saveHousingContainer( ident, container );
    invMgr.updateHousingItemPosition( item );

    zone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
    assert( zone );

    zone->spawnHousingObject( containerIdx, static_cast< uint16_t >( freeSlot ), containerId, item );

    return true;
  }

  return false;
}

Common::Furniture HousingMgr::getYardObjectForItem( Inventory::HousingItemPtr item ) const
{
  Common::Furniture obj {};

  obj.pos[ 0 ] = Common::Util::floatToUInt16( item->getPos().x );
  obj.pos[ 1 ] = Common::Util::floatToUInt16( item->getPos().y );
  obj.pos[ 2 ] = Common::Util::floatToUInt16( item->getPos().z );
  obj.dir = Common::Util::floatToUInt16Rot( item->getRot() );
  obj.patternId = item->getAdditionalData();

  return obj;
}

void HousingMgr::sendInternalEstateInventoryBatch( Entity::Player& player, bool storeroom )
{
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto zone = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
  if( !zone )
    return;

  // todo: perms check

  Inventory::InventoryTypeList containerIds;

  if( storeroom )
    containerIds = m_internalStoreroomContainers;
  else
    containerIds = m_internalPlacedItemContainers;

  auto& invMgr = Common::Service< InventoryMgr >::ref();
  auto& containers = getEstateInventory( zone->getLandIdent() );

  for( auto containerId : containerIds )
  {
    auto container = containers.find( containerId );
    if( container == containers.end() )
      break;

    invMgr.sendInventoryContainer( player, container->second );
  }
}

void HousingMgr::reqMoveHousingItem( Entity::Player& player, Common::LandIdent ident, uint8_t slot, Common::FFXIVARR_POSITION3 pos, float rot )
{
  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );
  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );

  if( !land || !hasPermission( player, *land, 0 ) )
    return;

  Logger::debug( "Ident: {}, slot: {}, posx: {} posy: {} posz: {}", ident.landId, slot, pos.x, pos.y, pos.z );

  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  // todo: what happens when either of these fail? how does the server let the client know that the moment failed
  // as is, if it does fail, the client will be locked and unable to move any item until reentering the territory
  if( auto terri = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone ) )
  {
    moveInternalItem( player, ident, *terri, slot, pos, rot );
  }
  else if( auto terri = std::dynamic_pointer_cast< HousingZone >( pZone ) )
  {
    moveExternalItem( player, ident, slot - 0x88, *terri, pos, rot );
  }
}

bool HousingMgr::moveInternalItem( Entity::Player& player, Common::LandIdent ident, Territory::Housing::HousingInteriorTerritory& terri, uint8_t slot,
                                   Common::FFXIVARR_POSITION3 pos, float rot )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto containerIdx = static_cast< uint16_t >( slot / 50 );
  auto slotIdx = slot % 50;

  uint16_t containerId = 0;
  try
  {
    containerId = m_internalPlacedItemContainers.at( containerIdx );
  }
  catch( const std::out_of_range& )
  {
    return false;
  }

  auto& containers = getEstateInventory( ident );

  auto it = containers.find( containerId );
  if( it == containers.end() )
    return false;

  auto container = it->second;

  auto item = std::dynamic_pointer_cast< Inventory::HousingItem >( container->getItem( static_cast< uint8_t >( slotIdx ) ) );
  if( !item )
    return false;

  item->setPos( pos );
  item->setRot( rot );

  // save
  auto& invMgr = Common::Service< InventoryMgr >::ref();
  invMgr.updateHousingItemPosition( item );

  terri.updateHousingObjectPosition( player, slot, item->getPos(), static_cast< uint16_t >( item->getRot() ) );

  // send confirmation to player
  uint32_t param1 = static_cast< uint32_t >( ( ident.landId << 16 ) | containerId );

  Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::HousingItemMoveConfirm, param1, slotIdx );

  return true;
}

bool HousingMgr::moveExternalItem( Entity::Player& player, Common::LandIdent ident, uint8_t slot, HousingZone& terri, Common::FFXIVARR_POSITION3 pos, float rot )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto land = terri.getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land || !hasPermission( player, *land, 0 ) )
    return false;

  auto& containers = getEstateInventory( ident );
  auto it = containers.find( Common::InventoryType::HousingExteriorPlacedItems );
  if( it == containers.end() )
    return false;

  auto container = it->second;

  auto item = std::dynamic_pointer_cast< Inventory::HousingItem >( container->getItem( slot ) );
  if( !item )
    return false;

  item->setPos( pos );
  item->setRot( rot );

  auto& invMgr = Common::Service< InventoryMgr >::ref();
  invMgr.updateHousingItemPosition( item );

  terri.updateYardObjectPos( player, slot, static_cast< uint16_t >( ident.landId ), *item );

  uint32_t param1 = static_cast< uint32_t >( ( ident.landId << 16 ) | Common::InventoryType::HousingExteriorPlacedItems );
  Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::HousingItemMoveConfirm, param1, slot );

  return true;
}

void HousingMgr::reqRemoveHousingItem( Entity::Player& player, uint16_t plot, uint16_t containerId, uint8_t slot, bool sendToStoreroom )
{
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  if( auto terri = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone ) )
  {
    auto ident = terri->getLandIdent();
    auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

    auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
    auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

    auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
    if( !land || !hasPermission( player, *land, 0 ) )
      return;

    removeInternalItem( player, *terri, containerId, slot, sendToStoreroom );
  }
  else if( auto terri = std::dynamic_pointer_cast< HousingZone >( pZone ) )
  {
    auto land = terri->getLand( static_cast< uint8_t >( plot ) );
    if( !land || !hasPermission( player, *land, 0 ) )
      return;

    auto containerType = static_cast< Common::InventoryType >( containerId );

    removeExternalItem( player, *terri, *land, containerType, slot, sendToStoreroom );
  }
}

bool HousingMgr::removeInternalItem( Entity::Player& player, Territory::Housing::HousingInteriorTerritory& terri, uint16_t containerId, uint16_t slotId,
                                     bool sendToStoreroom )
{
  auto& containers = getEstateInventory( terri.getLandIdent() );

  int8_t containerIdx = 0;

  if( isPlacedItemsInventory( static_cast< Common::InventoryType >( containerId ) ) )
  {
    for( auto cId : m_internalPlacedItemContainers )
    {
      if( containerId == cId )
        break;

      containerIdx++;
    }
  }
  else
    containerIdx = -1;

  // its possible to remove an item from any container in basically all these remove functions
  // eg, remove a permit and reuse it elsewhere
  // I'm not going to bother fixing it for now, but worth noting for future reference

  auto it = containers.find( containerId );
  if( it == containers.end() )
    return false;

  auto container = it->second;

  auto item = std::dynamic_pointer_cast< Inventory::HousingItem >( container->getItem( static_cast< uint8_t >( slotId ) ) );
  if( !item )
    return false;

  if( !sendToStoreroom )
  {
    // make sure the player has a free inv slot first
    Inventory::InventoryContainerPair containerPair;
    if( !player.getFreeInventoryContainerSlot( containerPair ) )
      return false;

    auto& invMgr = Common::Service< InventoryMgr >::ref();

    // remove it from housing inventory
    container->removeItem( static_cast< uint8_t >( slotId ) );
    invMgr.sendInventoryContainer( player, container );
    invMgr.removeHousingItemPosition( *item );
    invMgr.removeItemFromHousingContainer( terri.getLandIdent(), containerId, slotId );

    // add to player inv
    player.insertInventoryItem( containerPair.first, containerPair.second, item );

    // todo: set item as bound/unsellable/untradable
  }
  else
  {
    ItemContainerPtr freeContainer;
    Inventory::InventoryContainerPair freeSlotPair;
    freeContainer = getFreeEstateInventorySlot( terri.getLandIdent(), freeSlotPair, m_internalStoreroomContainers );

    if( !freeContainer )
      return false;

    auto& invMgr = Common::Service< InventoryMgr >::ref();

    container->removeItem( static_cast< uint8_t >( slotId ) );
    invMgr.sendInventoryContainer( player, container );
    invMgr.removeHousingItemPosition( *item );
    invMgr.removeItemFromHousingContainer( terri.getLandIdent(), containerId, slotId );

    freeContainer->setItem( static_cast< uint8_t >( slotId ), item );
    invMgr.sendInventoryContainer( player, freeContainer );
    invMgr.saveHousingContainer( terri.getLandIdent(), freeContainer );
  }

  // despawn
  if( containerIdx != -1 )
  {
    auto arraySlot = ( containerIdx * 50 ) + slotId;
    terri.removeHousingObject( static_cast< uint16_t >( arraySlot ) );
  }

  return true;
}

bool HousingMgr::removeExternalItem( Entity::Player& player, HousingZone& terri, Land& land, Common::InventoryType containerType, uint8_t slotId,
                                     bool sendToStoreroom )
{
  auto& containers = getEstateInventory( land.getLandIdent() );

  auto it = containers.find( containerType );
  if( it == containers.end() )
    return false;

  auto& sourceContainer = it->second;

  auto item = std::dynamic_pointer_cast< Inventory::HousingItem >( sourceContainer->getItem( slotId ) );
  if( !item )
    return false;

  bool shouldDespawnItem = containerType != Common::InventoryType::HousingExteriorStoreroom;

  auto& invMgr = Common::Service< InventoryMgr >::ref();

  if( sendToStoreroom )
  {
    auto& storeroomContainer = containers[ containerType ];
    auto freeSlot = storeroomContainer->getFreeSlot();

    if( freeSlot == -1 )
      return false;

    sourceContainer->removeItem( slotId );
    invMgr.sendInventoryContainer( player, sourceContainer );
    invMgr.removeItemFromHousingContainer( land.getLandIdent(), sourceContainer->getId(), slotId );
    invMgr.removeHousingItemPosition( *item );

    storeroomContainer->setItem( static_cast< uint8_t >( freeSlot ), item );
    invMgr.sendInventoryContainer( player, storeroomContainer );
    invMgr.saveHousingContainer( land.getLandIdent(), storeroomContainer );
  }
  else
  {
    Inventory::InventoryContainerPair containerPair;
    if( !player.getFreeInventoryContainerSlot( containerPair ) )
      return false;

    // remove from housing inv
    sourceContainer->removeItem( slotId );
    invMgr.sendInventoryContainer( player, sourceContainer );
    invMgr.removeHousingItemPosition( *item );
    invMgr.removeItemFromHousingContainer( land.getLandIdent(), sourceContainer->getId(), slotId );

    // add to player inv
    player.insertInventoryItem( containerPair.first, containerPair.second, item );
  }

  if( shouldDespawnItem )
    terri.despawnYardObject( land.getLandIdent().landId, slotId );

  return true;
}

ItemContainerPtr HousingMgr::getFreeEstateInventorySlot( Common::LandIdent ident, Inventory::InventoryContainerPair& pair, Inventory::InventoryTypeList bagList )
{
  auto& estateContainers = getEstateInventory( ident );

  for( auto bag : bagList )
  {
    auto it = estateContainers.find( bag );
    if( it == estateContainers.end() )
      continue;

    auto container = it->second;

    auto freeSlot = container->getFreeSlot();

    if( freeSlot == -1 )
      continue;

    pair = std::make_pair( bag, freeSlot );
    return container;
  }

  return nullptr;
}

void HousingMgr::reqEstateExteriorRemodel( Entity::Player& player, uint16_t plot )
{
  auto& server = Common::Service< World::WorldServer >::ref();

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto terri = std::dynamic_pointer_cast< HousingZone >( pZone );
  if( !terri )
    return;

  auto land = terri->getLand( static_cast< uint8_t >( plot ) );
  if( !land )
    return;

  if( !hasPermission( player, *land, 0 ) )
    return;

  auto& inv = getEstateInventory( land->getLandIdent() );
  auto it = inv.find( Common::InventoryType::HousingExteriorAppearance );
  if( it == inv.end() )
    return;

  auto& invMgr = Common::Service< InventoryMgr >::ref();

  invMgr.sendInventoryContainer( player, it->second );

  Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::ShowEstateExternalAppearanceUI, plot );
}

void HousingMgr::reqEstateInteriorRemodel( Entity::Player& player )
{
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto& server = Common::Service< World::WorldServer >::ref();

  auto terri = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
  if( !terri )
    return;

  auto ident = terri->getLandIdent();
  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );

  if( !land )
    return;

  if( !hasPermission( player, *land, 0 ) )
    return;

  auto& inv = getEstateInventory( land->getLandIdent() );
  auto it = inv.find( Common::InventoryType::HousingInteriorAppearance );
  if( it == inv.end() )
    return;

  auto& invMgr = Common::Service< InventoryMgr >::ref();

  invMgr.sendInventoryContainer( player, it->second );

  Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::ShowEstateInternalAppearanceUI );
}

void HousingMgr::reqRemodelEstateExerior( Entity::Player& player, Common::LandIdent landIdOrIndex, 
                                          const uint16_t StorageId[9], const int16_t ContainerIndex[9] )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto pSession = server.getSession( player.getCharacterId() );

  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto terri = std::dynamic_pointer_cast< HousingZone >( pZone );
  if( !terri )
    return;

  auto land = terri->getLand( static_cast< uint8_t >( landIdOrIndex.landId ) );
  if( !land )
    return;

  if( !hasPermission( player, *land, 0 ) )
    return;

  auto& inv = getEstateInventory( land->getLandIdent() );
  auto it = inv.find( Common::InventoryType::HousingExteriorAppearance );
  if( it == inv.end() )
    return;

  auto& invMgr = Common::Service< InventoryMgr >::ref();
  auto container = it->second;

  for( auto idx = 0; idx < 9; idx++ )
  {
    auto containerId = StorageId[ idx ];
    auto slotId = ContainerIndex[ idx ];

    Inventory::HousingItemPtr item;

    if( containerId == Common::InventoryType::Bag0 ||
        containerId == Common::InventoryType::Bag1 ||
        containerId == Common::InventoryType::Bag2 ||
        containerId == Common::InventoryType::Bag3 )
    {
      item = getHousingItemFromPlayer( player, static_cast< Common::InventoryType >( containerId ), slotId );
      if( !item )
        return;

      container->setItem( idx, item );
      invMgr.saveItem( player, item );
    }
  }

  invMgr.sendInventoryContainer( player, container );
  invMgr.saveHousingContainer( land->getLandIdent(), container );
  updateHouseModels( land->getHouse() );
}

void HousingMgr::reqRemodelEstateInterior( Entity::Player& player, Common::LandIdent landIdOrIndex, 
                                          const uint16_t StorageId[10], const int16_t ContainerIndex[10] )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto pSession = server.getSession( player.getCharacterId() );
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );

  auto terri = std::dynamic_pointer_cast< Territory::Housing::HousingInteriorTerritory >( pZone );
  if( !terri )
    return;

  auto ident = terri->getLandIdent();
  auto landSetId = toLandSetId( ident.territoryTypeId, ident.wardNum );

  auto pTeri = teriMgr.getTerritoryByGuId( landSetId );
  auto hZone = std::dynamic_pointer_cast< HousingZone >( pTeri );

  auto land = hZone->getLand( static_cast< uint8_t >( ident.landId ) );
  if( !land )
    return;

  if( !hasPermission( player, *land, 0 ) )
    return;

  auto& inv = getEstateInventory( land->getLandIdent() );
  auto it = inv.find( Common::InventoryType::HousingInteriorAppearance );
  if( it == inv.end() )
    return;

  auto& invMgr = Common::Service< InventoryMgr >::ref();
  auto container = it->second;

  for( auto idx = 0; idx < 10; idx++ )
  {
    auto containerId = StorageId[ idx ];
    auto slotId = ContainerIndex[ idx ];

    Inventory::HousingItemPtr item;

    if( containerId == Common::InventoryType::Bag0 ||
        containerId == Common::InventoryType::Bag1 ||
        containerId == Common::InventoryType::Bag2 ||
        containerId == Common::InventoryType::Bag3 )
    {
      item = getHousingItemFromPlayer( player, static_cast< Common::InventoryType >( containerId ), slotId );
      if( !item )
        return;

      container->setItem( idx, item );
      invMgr.saveItem( player, item );
    }
  }

  invMgr.sendInventoryContainer( player, container );
  invMgr.saveHousingContainer( ident, container );
  updateHouseModels( land->getHouse() );
}

bool HousingMgr::hasPermission( Entity::Player& player, Sapphire::Land& land, uint32_t permission )
{
  // todo: proper perms checks pls
  if( land.getOwnerId() == player.getCharacterId() )
    return true;

  // todo: check perms here

  return false;
}

Inventory::HousingItemPtr HousingMgr::getHousingItemFromPlayer( Entity::Player& player, Common::InventoryType type, uint8_t slot )
{
  auto tmpItem = player.dropInventoryItem( type, slot );
  if( !tmpItem )
    return nullptr;

  return Inventory::make_HousingItem( tmpItem->getUId(), tmpItem->getId() );
}

void HousingMgr::removeHouse( Entity::Player& player, uint16_t plot )
{
  auto& teriMgr = Common::Service< TerritoryMgr >::ref();
  auto pZone = teriMgr.getTerritoryByGuId( player.getTerritoryId() );
  if( !pZone )
    return;
  auto terri = std::dynamic_pointer_cast< HousingZone >( pZone );
  if( !terri )
    return;

  auto& server = Common::Service< World::WorldServer >::ref();

  auto pLand = terri->getLand( plot );
  if( !pLand )
    return;

  // can't remove when you are not the owner
  // TODO: actually use permissions here for FC houses
  if( !hasPermission( player, *pLand, 0 ) )
  {
    Network::Util::Packet::sendActorControlSelf( player, player.getId(), ActorControl::LogMsg, 3305 );
    return;
  }

  auto land = terri->getLand( static_cast< uint8_t >( plot ) );
  if( !land || !land->getHouse() )
    return;

  auto& interiorContainer = getEstateInventory( land->getLandIdent() )[ Common::InventoryType::HousingInteriorAppearance ];
  auto& invMgr = Common::Service< InventoryMgr >::ref();

  std::unordered_map< Common::InventoryType, ItemContainerPtr > changedContainerSet = {};

  for( int i = 0; i < interiorContainer->getMaxSize(); i++ )
  {
    auto item = interiorContainer->getItem( i );
    if( !item )
      continue;

    Inventory::InventoryContainerPair freeSlotPair;
    auto freeContainer = getFreeEstateInventorySlot( land->getLandIdent(), freeSlotPair, m_internalStoreroomContainers );
    if ( !freeContainer )
    {
      // not sure what to do
      interiorContainer->removeItem( i, true );
    }
    else
    {
      interiorContainer->removeItem( i, false );
      freeContainer->setItem( freeSlotPair.second , item );
      changedContainerSet[ freeSlotPair.first ] = freeContainer;
    }
  }

  invMgr.sendInventoryContainer( player, interiorContainer );
  invMgr.saveHousingContainer( land->getLandIdent(), interiorContainer );
  for( auto& entry : changedContainerSet )
  {
    invMgr.sendInventoryContainer( player, entry.second );
    invMgr.saveHousingContainer( land->getLandIdent(), entry.second );
  }

  deleteHouse( land->getHouse() );
  land->setHouse( nullptr );

  land->setStatus( Common::HouseStatus::Sold );
  land->updateLandDb();
  terri->sendLandUpdate( plot );

  player.setLandFlags( Common::LandFlagsSlot::Private, Common::HOUSING_LAND_STATUS::HOUSING_LAND_STATUS_NOINIT, land->getLandIdent() );
  sendLandFlagsSlot( player, Common::LandFlagsSlot::Private );

  terri->removeEstateEntranceEObj( plot );
  return;
}

void HousingMgr::sendLandFlagsSlot( Entity::Player& player, Common::LandFlagsSlot slot )
{
  auto& server = Common::Service< World::WorldServer >::ref();
  auto landFlags = makeZonePacket< FFXIVIpcCharaHousingLandData >( player.getId() );

  Common::LandType type;

  switch( slot )
  {
    case Common::LandFlagsSlot::Private:
      type = Common::LandType::Private;
      break;

    case Common::LandFlagsSlot::FreeCompany:
      type = Common::LandType::FreeCompany;
      break;

    default:
      // todo: other/unsupported land types
      return;
  }

  auto landData = player.getCharaLandData( slot );
  landFlags->data().Index = static_cast< uint32_t >( type );
  landFlags->data().LandData.landId = landData.landId;
  landFlags->data().LandData.landFlags = landData.landFlags;

  server.queueForPlayer( player.getCharacterId(), landFlags );
}

void HousingMgr::sendLandFlags( Entity::Player& player )
{
  if( Sapphire::LandPtr pLand = getLandByOwnerId( player.getCharacterId() ) )
  {
    uint32_t state = 0;
    if( pLand->getHouse() )
    {
      state |= Common::LandFlags::CHARA_HOUSING_LAND_DATA_FLAG_HOUSE;

      // todo: remove this, debug for now
      state |= Common::LandFlags::CHARA_HOUSING_LAND_DATA_FLAG_AETHERYTE;
    }

    player.setLandFlags( Common::LandFlagsSlot::Private, state, pLand->getLandIdent() );
  }

  auto landFlags = makeZonePacket< FFXIVIpcCharaHousing >( player.getId() );

  landFlags->data().FcLands = player.getCharaLandData( Common::LandFlagsSlot::FreeCompany );
  landFlags->data().CharaLands = player.getCharaLandData( Common::LandFlagsSlot::Private );

  auto& server = Common::Service< World::WorldServer >::ref();
  server.queueForPlayer( player.getCharacterId(), landFlags );
}