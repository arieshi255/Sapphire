// This is an automatically generated C++ script template
// Content needs to be added by hand to make it function
// In order for this script to be loaded, move it to the correct folder in <root>/scripts/

#include <Actor/Player.h>
#include "Manager/EventMgr.h"
#include <ScriptObject.h>
#include <Service.h>

// Script: CmnGscGilToCoin_00240
// Event Name: Exchanging Gil for MGP
// Event ID: 721136
using namespace Sapphire;

class CmnGscGilToCoin : public Sapphire::ScriptAPI::EventScript
{
  private:
    // Basic event information 
    // Entities found in the script data of the event
    static constexpr auto ClearQuest = 65970;

  public:
    CmnGscGilToCoin() : Sapphire::ScriptAPI::EventScript( 721136 ){}; 
    ~CmnGscGilToCoin() = default; 

  //////////////////////////////////////////////////////////////////////
  // Event Handlers
  void onTalk( uint32_t eventId, Entity::Player& player, uint64_t actorId ) override
  {
    Scene00000( player );
  }


  private:
  //////////////////////////////////////////////////////////////////////
  // Available Scenes in this event, not necessarly all are used
  //////////////////////////////////////////////////////////////////////

  void Scene00000( Entity::Player& player )
  {
    eventMgr().playScene( player, getId(), 0, NONE, bindSceneReturn( &CmnGscGilToCoin::Scene00000Return ) );
  }

  void Scene00000Return( Entity::Player& player, const Event::SceneResult& result )
  {
    // Convert gil to mgp
    // arg0: proceed
    // arg1: mgp to get
    if( result.getResult( 0 ) == 1 )
    {
      auto mgpToGet = result.getResult( 1 );
      player.removeCurrency( Common::CurrencyType::Gil, mgpToGet * 10 );
      player.addCurrency( Common::CurrencyType::Mgp, mgpToGet );
    }
  }

};

EXPOSE_SCRIPT( CmnGscGilToCoin );