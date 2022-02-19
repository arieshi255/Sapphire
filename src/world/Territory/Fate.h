#ifndef SAPPHIRE_FATE_H
#define SAPPHIRE_FATE_H

#include "Territory.h"
#include "Event/Director.h"
#include "Forwards.h"
#include <Exd/Structs.h>

namespace Sapphire
{
  class Fate
  {
  public:
    enum FateState
    {
      Created,
      InProgress,
      Finished,
      Failed,
      Reset,
      Expired
    };

    Fate( uint32_t fateId, uint16_t zoneId );

    bool init();

  private:
    uint32_t m_fateId;
    uint16_t m_zoneId;

    FateState m_state;
  };
}

#endif