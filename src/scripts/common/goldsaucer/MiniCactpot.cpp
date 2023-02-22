#include <ScriptObject.h>
#include <Actor/Player.h>

#include <Exd/ExdData.h>

using namespace Sapphire;

class MiniCactpot : public Sapphire::ScriptAPI::EventScript
{
public:
  MiniCactpot() :
    Sapphire::ScriptAPI::EventScript( 2424832 )
  {
  }

  void onTalk( uint32_t eventId, Entity::Player& player, uint64_t actorId ) override
  {
    eventMgr().playScene( player, getId(), 0, HIDE_HOTBAR | NO_DEFAULT_CAMERA, { 5, 19, 10, 121783, 3460, 3 } );
  }

  void onYield( uint32_t eventId, uint16_t sceneId, uint8_t yieldId, Entity::Player& player, const std::string& resultString, uint64_t resultInt, const std::vector< uint32_t >& results ) override
  {
    switch( yieldId )
    {
      case 5:
      {
        eventMgr().resumeScene( player, eventId, sceneId, 19, 
          { 3460, 1, 0, 8, 10000, 36, 720, 360, 80, 252, 108, 72, 54, 180, 72, 180, 119, 36, 306, 1080, 144, 1800, 3600 } );
        break;
      }
    }
  }
};

EXPOSE_SCRIPT( MiniCactpot );