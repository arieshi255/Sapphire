#pragma once

#include <Common.h>
#include "ForwardsZone.h"
#include "FateMgr.h"

#include "Territory/Territory.h"

#include <bitset>
#include <map>

namespace Sapphire::World::Manager
{

  class MapMgr
  {
  public:
    enum UpdateMode : uint8_t
    {
      Quest = 1,
      GuildLeveAssignment = 2,
      GuildOrderGuide = 4,
      TripleTriad = 8,
      CustomTalk = 16,
      PreHandler = 32,

      Fates = 0x0F,

      All = 0x3F
    };

    MapMgr();

    void updateFate( Entity::Player& player, Fate& fate );

    void updateFates( TerritoryPtr zone, std::map< uint32_t, FatePtr >& fateZoneData, uint32_t playerId = 0 );

    void updateAll( Entity::Player& player, TerritoryPtr zone );

  private:
    struct EventData
    {
      uint32_t iconId;
      uint32_t layoutId;
      uint32_t handlerId;
    };

    struct less
    {
      constexpr bool operator()( const uint32_t& _Left, const uint32_t& _Right ) const
      {
        const uint16_t left = _Left;
        const uint16_t right = _Right;

        if( left == right )
        {
          const uint16_t typeLeft = _Left >> 16;
          const uint16_t typeRight = _Right >> 16;

          return typeLeft < typeRight;
        }

        return left < right;
      }
    };

    std::map< uint16_t, std::multimap< uint32_t, EventData, less > > m_mapData;

    void fillPacket( std::multimap< uint32_t, EventData, less >& mapData, uint32_t* iconIds, uint32_t* levelIds, uint32_t* eventIds );
    void sendPackets( TerritoryPtr zone, std::multimap< uint32_t, EventData, less >& mapData, UpdateMode updateMode, uint32_t playerId = 0 );
  };

}