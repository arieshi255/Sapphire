
#include <Network/CommonActorControl.h>

#include <Manager/TerritoryMgr.h>

#include "Fate.h"

using namespace Sapphire::Common;
using namespace Sapphire::Network::ActorControl;
using namespace Sapphire::World::Manager;

Sapphire::Fate::Fate( uint32_t fateId, uint16_t zoneId ) :
  m_fateId( fateId ),
  m_zoneId( zoneId ),
  m_state( Created )
{

}

bool Sapphire::Fate::init()
{
  return true;
}