#include "RespawnBNpcTask.h"

#include <Logging/Logger.h>
#include <Actor/BNpc.h>
#include <Manager/TerritoryMgr.h>
#include <Service.h>

#include <Territory/Territory.h>
#include <utility>

using namespace Sapphire::World;

RespawnBNpcTask::RespawnBNpcTask( uint64_t delayTime, uint16_t zoneId, uint32_t layoutId, std::list< uint32_t > hateIds ) :
  Task( delayTime ),
  m_zoneId( zoneId ),
  m_layoutId( layoutId ),
  m_hateIds( hateIds )
{
}

void RespawnBNpcTask::onQueue()
{
  Logger::debug( { __FUNCTION__ } );
}

void RespawnBNpcTask::execute()
{

  auto& teriMgr = Common::Service< World::Manager::TerritoryMgr >::ref();
  auto pZone = teriMgr.getZoneByTerritoryTypeId( m_zoneId );

  if( !pZone )
    return;

  auto bnpc = pZone->createBNpcFromLayoutId( m_layoutId );

  for( const auto hateId : m_hateIds )
  {
    auto target = pZone->getActiveBNpcByLayoutId( hateId );
    bnpc->hateListAdd( target, 1 );
  }
}

std::string RespawnBNpcTask::toString()
{
  return fmt::format( "RespawnBNpcTask: BNpc#{}, TerritoryTypeId#{}, ElapsedTimeMs: {}", m_layoutId, m_zoneId, getDelayTimeMs() );
}