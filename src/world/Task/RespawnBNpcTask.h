#pragma once

#include <cstdint>
#include <string>
#include <list>
#include <ForwardsZone.h>
#include "Task.h"

namespace Sapphire::World
{

class RespawnBNpcTask : public Task
{
public:
  RespawnBNpcTask( uint64_t delayTime, uint16_t zoneId, uint32_t layoutId, std::list< uint32_t > hateIds = {} );

  void onQueue() override;
  void execute() override;
  std::string toString() override;
private:
  uint16_t m_zoneId;
  uint32_t m_layoutId;
  std::list< uint32_t > m_hateIds;
};

template< typename... Args >
std::shared_ptr< RespawnBNpcTask > makeRespawnBNpcTask( Args... args )
{
  return std::make_shared< RespawnBNpcTask >( args... );
}

}
