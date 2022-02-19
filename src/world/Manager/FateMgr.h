#pragma once

#include "Territory/Fate.h"
#include "Event/Director.h"
#include "Forwards.h"
#include <Exd/Structs.h>

namespace Sapphire::World::Manager
{
  class FateMgr
  {
  public:
    struct FateData
    {
      uint32_t iconId;
      uint32_t layoutId;
      uint32_t handlerId;
    };

    FateMgr();

    void init();

    void spawnFate( Entity::Player& player, uint32_t fateId );

    uint8_t getFateCount( uint16_t zoneId );

  private:
    enum FateState
    {
      Active = 2,
      Completed = 4,
      Failed,
      Preparing1,
      Preparing = 7,
      Finished
    };

    std::map< uint16_t, std::map< uint32_t, FateData > > m_fateZoneMap;
    std::map< uint16_t, std::map< uint32_t, Fate > > m_spawnedFates;
  };
}