#ifndef SAPPHIRE_FATE_H
#define SAPPHIRE_FATE_H

#include "Territory.h"
#include "Event/FateDirector.h"

#include "Forwards.h"
#include <Common.h>
#include <Exd/Structs.h>

namespace Sapphire
{
  class Fate : public Event::FateDirector
  {
  public:
    Fate( uint32_t fateId, uint16_t zoneId, uint32_t startTime, Common::FateData fateData );

    Common::FateData getFateData();

    uint16_t getZoneId();
    uint32_t getFateId();
    uint32_t getStartTime();
    uint32_t getLimitTime();
    uint32_t getEndTime();

    bool init();

  private:
    uint32_t m_fateId;
    uint16_t m_zoneId;

    Common::FateState m_state;
    Common::FateData m_fateData;

    uint32_t m_startTime;
    uint32_t m_limitTime;
  };
  
}

#endif