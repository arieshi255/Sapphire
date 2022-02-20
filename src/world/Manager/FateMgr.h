#pragma once

#include <Common.h>
#include "ForwardsZone.h"
#include "Territory/Fate.h"
namespace Sapphire::World::Manager
{
  class FateMgr
  {
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

  public:
    FateMgr();

    void onUpdate( uint64_t tick );

    void onPlayerZoneIn( Entity::Player& player );

    void sendSyncPacket( Entity::Player& player, Fate& fate );

    void despawnFate( Fate& fate, FateState state );

    void spawnFate( Entity::Player& player, uint32_t fateId );

    uint8_t getFateCount( uint16_t zoneId );

  private:
    std::map< uint16_t, std::map< uint32_t, Common::FateData > > m_fateZoneMap;
    std::map< uint16_t, std::map< uint32_t, Sapphire::FatePtr > > m_spawnedFates;

    Event::FateDirector m_globalDirector;
  };
}