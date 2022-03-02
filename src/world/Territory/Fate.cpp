#include <Service.h>
#include <Logging/Logger.h>
#include <Actor/BNpc.h>
#include <Actor/Player.h>
#include <Util/UtilMath.h>

#include <Network/CommonActorControl.h>
#include <Network/PacketWrappers/ActorControlSelfPacket.h>

#include <Manager/TerritoryMgr.h>
#include <Manager/TaskMgr.h>
#include <Manager/RNGMgr.h>

#include <Task/RespawnBNpcTask.h>

#include <WorldServer.h>

#include "Fate.h"

using namespace Sapphire::Common;
using namespace Sapphire::Network::ActorControl;
using namespace Sapphire::Network::Packets::WorldPackets::Server;
using namespace Sapphire::World::Manager;

Sapphire::Fate::Fate( uint32_t fateId, uint16_t zoneId, uint32_t startTime, FateData fateData ) :
  FateDirector( fateId ),
  m_fateId( fateId ),
  m_zoneId( zoneId ),
  m_fateData( fateData ),
  m_startTime( startTime ),
  m_state( FateStatus::Preparing )
{
  m_limitTime = fateData.timeLimit;
  m_radius = Radius; // TODO: account for scale?
}

Sapphire::Fate::~Fate()
{
  auto& terriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto zone = terriMgr.getZoneByTerritoryTypeId( m_zoneId );

  for( const auto enemyId : m_fateEnemies )
  {
    zone->removeActor( zone->getActiveBNpcByLayoutId( enemyId ) );
  }
}

FateStatus Sapphire::Fate::getFateStatus()
{
  return m_state;
}

FateData Sapphire::Fate::getFateData()
{
  return m_fateData;
}

uint16_t Sapphire::Fate::getZoneId()
{
  return m_zoneId;
}

uint32_t Sapphire::Fate::getFateId()
{
  return m_fateId;
}

uint32_t Sapphire::Fate::getEndTime()
{
  return m_startTime + m_limitTime;
}

uint32_t Sapphire::Fate::getStartTime()
{
  return m_startTime;
}

uint32_t Sapphire::Fate::getLimitTime()
{
  return m_limitTime;
}

float Sapphire::Fate::getRadius()
{
  return m_radius;
}

uint8_t Sapphire::Fate::getProgress()
{
  return m_progress;
}

void Sapphire::Fate::onUpdate( uint64_t tick )
{
  if( m_state == FateStatus::Active )
  {
    for( const auto playerId : m_fatePlayers )
    {
      auto& server = Common::Service< World::WorldServer >::ref();
      auto player = server.getPlayer( playerId );

      if( Util::distance( m_fateData.transform, player->getPos() ) > m_radius )
      {
        Logger::debug( "[Fate #{}]: Player #{} left FATE!", m_fateId, playerId );
        Logger::debug( "{}", Util::distance( m_fateData.transform, player->getPos() ) );
        m_fatePlayers.remove( playerId );
        return;
      }
    }
  }
}

void Sapphire::Fate::updateProgress( uint8_t progress )
{
  m_progress += progress;

  if( m_progress >= 100 )
  {
    m_progress = 100;
    m_state = FateStatus::Completed;
  }

  auto& terriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto zone = terriMgr.getZoneByTerritoryTypeId( m_zoneId );

  zone->queuePacketForZone( makeActorControlSelf( 0, 0x09B, m_fateId, m_progress ) );
}

void Sapphire::Fate::updateHate()
{
  auto& terriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto zone = terriMgr.getZoneByTerritoryTypeId( m_zoneId );

  // Add all allies to each enemy's hate list
  for( const auto enemyId : m_fateEnemies )
    for( const auto allyId : m_fateAllies )
      zone->getActiveBNpcByLayoutId( enemyId )->hateListAdd( zone->getActiveBNpcByLayoutId( allyId ), 1 );
}

void Sapphire::Fate::onBNpcKill( uint32_t layoutId, Common::BNpcType type )
{
  Logger::debug( "[Fate #{}]: BNpc #{} killed!", m_fateId, layoutId );

  // If all allies die, the FATE fails
  if( type != BNpcType::Enemy )
  {
    m_fateAllies.remove( layoutId );

    if( m_fateAllies.empty() )
      m_state = FateStatus::Failed;

    return;
  }

  // If the FATE is "kill enemies", update the progress per kill
  if( m_fateData.rule == FateRule::Kill )
    updateProgress( 3 );

  // If the FATE is "kill boss", update the progress
  // TODO: Make this update per attack and not all at once
  if( m_fateData.rule == FateRule::KillBoss )
    updateProgress( 100 );

  // This needs more work
  // If enemies should respawn (most do this)
  if( m_fateData.rule != FateRule::KillBoss && m_fateData.rule != FateRule::Escort )
  {
    auto& taskMgr = Common::Service< World::Manager::TaskMgr >::ref();
    taskMgr.queueTask( World::makeRespawnBNpcTask( 4000, m_zoneId, layoutId, m_fateAllies ) );
  }
  else
  {
    m_fateEnemies.remove( layoutId );
  }
}

void Sapphire::Fate::onPlayerEnter( Entity::Player& player )
{
  if( std::find( m_fatePlayers.begin(), m_fatePlayers.end(), player.getId() ) == m_fatePlayers.end() )
    m_fatePlayers.push_back( player.getId() );
  Logger::debug( "[Fate #{}]: Player #{} entered FATE!", m_fateId, player.getId() );
}

bool Sapphire::Fate::init()
{
  m_state = FateStatus::Active;

  auto& terriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto zone = terriMgr.getZoneByTerritoryTypeId( m_zoneId );

  auto ids = zone->getLayoutIdsByFateId( m_fateId );

  for( const auto id : ids )
  {
    auto type = zone->createBNpcFromLayoutId( id )->getEnemyType();
    if( type == BNpcType::Friendly )
      m_fateAllies.push_back( id );
    else
      m_fateEnemies.push_back( id );
  }

  updateHate();

  // TODO: Need to handle different kinds of fates better

  return true;
}