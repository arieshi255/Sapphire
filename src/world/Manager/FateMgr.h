#pragma once

#include <optional>

#include <Common.h>
#include "ForwardsZone.h"
#include "Territory/Fate.h"
namespace Sapphire::World::Manager
{
  class FateMgr
  {
  public:
    FateMgr();

    void onUpdate( uint64_t tick );

    void onPlayerZoneIn( Entity::Player& player );

    void sendSyncPacket( Fate& fate, uint32_t playerId = 0 );

    void queueFate( uint16_t zoneId, std::map< uint32_t, Common::FateData >& fateZoneData );

    void despawnFate( Fate& fate, Common::FateState state );

    void spawnFate( uint16_t zoneId, uint32_t fateId );

    std::optional< Sapphire::FatePtr > getFateById( uint32_t fateId );

  private:
    std::map< uint32_t, uint32_t > m_fateTimeMap = 
    {
      { 159, 2 },
      { 161, 2 },
      { 162, 2 },
      { 195, 3 },
      { 196, 2 },
      { 197, 2 },
      { 198, 2 },
      { 199, 2 },
      { 200, 2 },
      { 201, 2 },
      { 202, 2 },
      { 203, 2 },
      { 204, 2 },
      { 205, 2 },
      { 206, 2 },
      { 207, 2 },
      { 218, 2 },
      { 507, 2 },
      { 505, 2 },
      { 506, 2 },
      { 647, 2 },
      { 758, 2 },
      { 838, 2 },
      { 877, 2 },
      { 878, 2 },
      { 879, 2 },
      { 902, 2 },
      { 903, 2 },
      { 904, 2 },
      { 905, 2 },
      { 906, 2 },
      { 907, 2 }
    };

    struct ZoneData
    {
      uint32_t lastSpawn;
      uint32_t spawnTimer;
    };

    static constexpr auto DefaultWeight = 50;
    static constexpr auto SpawnTimeLow = 180; // 3 min
    static constexpr auto SpawnTimeHigh = 420; // 7 mim

    std::map< uint16_t, ZoneData > m_zoneSpawnMap;
    std::map< uint16_t, std::map< uint32_t, Common::FateData > > m_fateZoneMap;
    std::map< uint16_t, std::map< uint32_t, Sapphire::FatePtr > > m_spawnedFates;

    Event::FateDirector m_globalDirector;
  };
}