#include <Service.h>
#include <Logging/Logger.h>
#include <Actor/BNpc.h>

#include <Network/CommonActorControl.h>

#include <Manager/TerritoryMgr.h>
#include <Manager/RNGMgr.h>

#include "Fate.h"

using namespace Sapphire::Common;
using namespace Sapphire::Network::ActorControl;
using namespace Sapphire::World::Manager;

Sapphire::Fate::Fate( uint32_t fateId, uint16_t zoneId, uint32_t startTime, FateData fateData ) :
  FateDirector( fateId ),
  m_fateId( fateId ),
  m_zoneId( zoneId ),
  m_fateData( fateData ),
  m_startTime( startTime ),
  m_state( FateState::Preparing )
{
  m_limitTime = fateData.timeLimit;
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

void Sapphire::Fate::onUpdate( uint64_t tick )
{
  if( m_state == FateState::Active )
  {
    if( m_fateEnemies.size() < MaxEnemySpawn )
    {
      
    }
  }
}

void Sapphire::Fate::onBNpcKill( uint32_t instanceId )
{
  Logger::debug( "[Fate] #{}: BNpc #{} killed!", m_fateId, instanceId );
}

bool Sapphire::Fate::init()
{
  m_state = FateState::Active;

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

  // TODO: Need to handle different kinds of fates

  return true;
}