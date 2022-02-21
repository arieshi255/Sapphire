#include <Network/CommonActorControl.h>

#include <Manager/TerritoryMgr.h>

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

bool Sapphire::Fate::init()
{
  m_state = FateState::Active;

  return true;
}