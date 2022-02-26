#ifndef SAPPHIRE_FATE_H
#define SAPPHIRE_FATE_H

#include <list>

#include "Territory.h"
#include "Event/FateDirector.h"

#include "Forwards.h"
#include <Common.h>
#include <Exd/Structs.h>
#include <Actor/Player.h>

namespace Sapphire
{
  class Fate : public Event::FateDirector
  {
  public:
    Fate( uint32_t fateId, uint16_t zoneId, uint32_t startTime, Common::FateData fateData );
    ~Fate();

    Common::FateStatus getFateStatus();
    Common::FateData getFateData();

    uint16_t getZoneId();
    uint32_t getFateId();
    uint32_t getStartTime();
    uint32_t getLimitTime();
    uint32_t getEndTime();

    float getRadius();

    uint8_t getProgress();

    void updateProgress( uint8_t progress );

    virtual void onUpdate( uint64_t tick );

    virtual void onPlayerEnter( Entity::Player& player );

    virtual void onBNpcKill( uint32_t instanceId, Common::BNpcType type );

    virtual bool init();

  private:
    static constexpr auto MaxEnemySpawn = 5;
    static constexpr auto Radius = 55.0f;

    std::list< uint32_t > m_fatePlayers;
    std::list< uint32_t > m_fateEnemies;
    std::list< uint32_t > m_fateAllies;

    uint32_t m_fateId;
    uint16_t m_zoneId;

    Common::FateStatus m_state;
    Common::FateData m_fateData;

    uint32_t m_startTime;
    uint32_t m_limitTime;

    uint8_t m_progress;

    float m_radius;
  };
  
}

#endif