#pragma once

#include <Common.h>

#include "ForwardsZone.h"


namespace Sapphire
{
  namespace World::Manager
  {
    class DebugCommandMgr;
  }

  class DebugCommand
  {
  public:

    using pFunc = void ( World::Manager::DebugCommandMgr::* )( char*, Entity::Player&, std::shared_ptr< DebugCommand > );

    // String for the command
    std::string m_commandName;

    // command callback
    pFunc m_pFunc;

    // helptext
    std::string m_helpText;

    // userlevel needed to execute the command
    uint8_t m_gmLevel;

    DebugCommand( const std::string& n, pFunc functionPtr, const std::string& hText, uint8_t uLevel )
    {
      m_commandName = n;
      m_pFunc = functionPtr;
      m_helpText = hText;
      m_gmLevel = uLevel;
    }

    ~DebugCommand()
    {

    }

    const std::string& getName() const
    {
      return m_commandName;
    }

    const std::string& getHelpText() const
    {
      return m_helpText;
    }

    uint8_t getRequiredGmLevel() const
    {
      return m_gmLevel;
    }

  };

}