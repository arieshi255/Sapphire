#include <Script/NativeScriptApi.h>
#include <ScriptObject.h>
#include <Actor/Player.h>
#include <Action/CommonAction.h>
#include <Action/Action.h>

using namespace Sapphire;
using namespace Sapphire::World::Action;

class ActionInfuriate : public Sapphire::ScriptAPI::ActionScript
{
public:
  ActionInfuriate() : Sapphire::ScriptAPI::ActionScript( 52 )
  {
  }

  static constexpr auto Potency = 300;

  void onExecute( Sapphire::World::Action::Action& action ) override
  {
    auto pPlayer = action.getSourceChara()->getAsPlayer();

    if( !pPlayer )
      return;

    action.applyStatusEffectSelf( Infuriated );
    pPlayer->addStatusEffectByIdIfNotExist( Infuriated, 30000, *pPlayer->getAsChara() );
  }
};

EXPOSE_SCRIPT( ActionInfuriate );