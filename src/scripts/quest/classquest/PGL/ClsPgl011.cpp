// This is an automatically generated C++ script template
// Content needs to be added by hand to make it function
// In order for this script to be loaded, move it to the correct folder in <root>/scripts/

#include <Actor/Player.h>
#include "Manager/EventMgr.h"
#include <ScriptObject.h>
#include <Service.h>

// Quest Script: ClsPgl011_00532
// Quest Name: Way of the Pugilist
// Quest ID: 66068
// Start NPC: 1001286 (Gagaruna)
// End NPC: 1003817 (Hamon)

using namespace Sapphire;

class ClsPgl011 : public Sapphire::ScriptAPI::QuestScript
{
  private:
    // Basic quest information 
    // Quest vars / flags used
    // UI8AL

    /// Countable Num: 1 Seq: 255 Event: 1 Listener: 1003817
    // Steps in this quest ( 0 is before accepting, 
    // 1 is first, 255 means ready for turning it in
    enum Sequence : uint8_t
    {
      Seq0 = 0,
      SeqFinish = 255,
    };

    // Entities found in the script data of the quest
    static constexpr auto Actor0 = 1001286; // Gagaruna ( Pos: -88.966698 2.550000 -51.144600  Teri: 130 )
    static constexpr auto Actor1 = 1003817; // Hamon ( Pos: -74.541298 1.999990 -42.379501  Teri: 130 )
    static constexpr auto Classjob = 2;
    static constexpr auto GearsetUnlock = 1905;
    static constexpr auto LocAction1 = 112;
    static constexpr auto LocAction2 = 113;
    static constexpr auto LocAction3 = 961;
    static constexpr auto LogmessageMonsterNotePageUnlock = 1013;
    static constexpr auto UnlockImageClassPgl = 42;
    static constexpr auto UnlockImageGearSet = 29;

  public:
    ClsPgl011() : Sapphire::ScriptAPI::QuestScript( 66068 ){}; 
    ~ClsPgl011() = default; 

  //////////////////////////////////////////////////////////////////////
  // Event Handlers
  void onTalk( World::Quest& quest, Entity::Player& player, uint64_t actorId ) override
  {
    switch( actorId )
    {
      case Actor0:
      {
        if(quest.getSeq() == Seq0) {
          Scene00000(quest, player);
        }

        break;
      }
      case Actor1:
      {
        break;
      }
    }
  }


  private:
  //////////////////////////////////////////////////////////////////////
  // Available Scenes in this quest, not necessarly all are used
  //////////////////////////////////////////////////////////////////////

  void Scene00000( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 0, HIDE_HOTBAR, bindSceneReturn( &ClsPgl011::Scene00000Return ) );
  }

  void Scene00000Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
    if( result.getResult( 0 ) == 1 ) // accept quest
    {
      quest.setSeq(SeqFinish);
    }

  }

  //////////////////////////////////////////////////////////////////////

  void Scene00001( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 1, FADE_OUT | HIDE_UI, bindSceneReturn( &ClsPgl011::Scene00001Return ) );
  }

  void Scene00001Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {

    if( result.getResult( 0 ) == 1 )
    {
      player.finishQuest( getId(), result.getResult( 1 ) );
    }

  }

};

EXPOSE_SCRIPT( ClsPgl011 );