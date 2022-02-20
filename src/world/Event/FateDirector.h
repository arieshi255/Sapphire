#pragma once

#include <Common.h>

#include "ForwardsZone.h"
#include "Director.h"

namespace Sapphire::Event
{

  /*!
  \class Fate Director
  \brief Base class for all Directors implements sequence and variables

  */

  class FateDirector : public Director
  {
  public:
    enum FateDirectorState
    {
      Created,
      DutyReset,
      DutyInProgress,
      DutyFinished,
      DutyFailed
    };

    FateDirector();
    FateDirector( uint32_t fateId );
  };
}