// This is an automatically generated C++ script template
// Content needs to be added by hand to make it function
// In order for this script to be loaded, move it to the correct folder in <root>/scripts/

#include <Actor/Player.h>
#include <Actor/BNpc.h>
#include "Manager/EventMgr.h"
#include <ScriptObject.h>
#include <Service.h>

// Quest Script: ClsExc020_00311
// Quest Name: Way of the Marauder
// Quest ID: 65847
// Start NPC: 1000926
// End NPC: 1000927

using namespace Sapphire;

class ClsExc020 : public Sapphire::ScriptAPI::QuestScript
{
  private:
    // Basic quest information 
    // Quest vars / flags used
    // UI8AL
    // UI8BH
    // UI8BL

    /// Countable Num: 2 Seq: 1 Event: 1 Listener: 1000927
    /// Countable Num: 2 Seq: 2 Event: 5 Listener: 347
    /// Countable Num: 2 Seq: 2 Event: 5 Listener: 338
    /// Countable Num: 1 Seq: 2 Event: 5 Listener: 49
    /// Countable Num: 0 Seq: 255 Event: 1 Listener: 1000927
    // Steps in this quest ( 0 is before accepting, 
    // 1 is first, 255 means ready for turning it in
    enum Sequence : uint8_t
    {
      Seq0 = 0,
      Seq1 = 1,
      Seq2 = 2,
      SeqFinish = 255,
    };

    // Entities found in the script data of the quest
    static constexpr auto Actor0 = 1000926;
    static constexpr auto Actor1 = 1000927;
    static constexpr auto Enemy0 = 417;
    static constexpr auto Enemy1 = 392;
    static constexpr auto Enemy2 = 49;
    static constexpr auto LogmessageMonsterNotePageUnlock = 1015;
    static constexpr auto UnlockImageMonsterNote = 32;

  public:
    ClsExc020() : Sapphire::ScriptAPI::QuestScript( 65847 ){}; 
    ~ClsExc020() = default; 

  //////////////////////////////////////////////////////////////////////
  // Event Handlers
  void onTalk( World::Quest& quest, Entity::Player& player, uint64_t actorId ) override
  {
    switch( actorId )
    {
      case Actor0:
      {
        if( quest.getSeq() == Seq0 )
          Scene00000( quest, player );
        break;
      }
      case Actor1:
      {
        if( quest.getSeq() == Seq1 )
          Scene00003( quest, player );
        else if( quest.getSeq() == SeqFinish )
          Scene00004( quest, player );
        break;
      }
    }
  }

  void onBNpcKill( World::Quest& quest, Entity::BNpc& bnpc, Entity::Player& player ) override
  {
    switch( bnpc.getBNpcNameId() )
    {
      case Enemy0: { 
        if( quest.getUI8AL() != 3 )
        {
          quest.setUI8AL( quest.getUI8AL() + 1 );
          if( quest.getUI8AL() == 3 )
            checkQuestCompletion( quest, player, 1 );
          else
            eventMgr().sendEventNotice( player, getId(), 1, 2, quest.getUI8AL(), 3 );
        }
        break;
      }
      case Enemy1: {
        if( quest.getUI8BH() != 3 )
        {
          quest.setUI8BH( quest.getUI8BH() + 1 );
          if( quest.getUI8BH() == 3 )
            checkQuestCompletion( quest, player, 2 );
          else
            eventMgr().sendEventNotice( player, getId(), 2, 2, quest.getUI8BH(), 3 );
        }
        break;
      }
      case Enemy2: {
        if( quest.getUI8BL() != 3 )
        {
          quest.setUI8BL( quest.getUI8BL() + 1 );
          if( quest.getUI8BL() == 3 )
            checkQuestCompletion( quest, player, 3 );
          else
            eventMgr().sendEventNotice( player, getId(), 3, 2, quest.getUI8BL(), 3 );
        }
        break;
      }
    }
  }

  private:
  void checkQuestCompletion( World::Quest& quest, Entity::Player& player, uint32_t varIdx )
  {
    eventMgr().sendEventNotice( player, getId(), varIdx, 0, 0, 0 );

    if( quest.getUI8AL() == 3 && quest.getUI8BH() == 3 && quest.getUI8BL() == 3 )
    {
      quest.setSeq( SeqFinish );
    }
  }

  //////////////////////////////////////////////////////////////////////
  // Available Scenes in this quest, not necessarly all are used
  //////////////////////////////////////////////////////////////////////

  void Scene00000( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 0, HIDE_HOTBAR, bindSceneReturn( &ClsExc020::Scene00000Return ) );
  }

  void Scene00000Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
    if( result.getResult( 0 ) == 1 ) // accept quest
    {
      Scene00001( quest, player );
    }
  }

  //////////////////////////////////////////////////////////////////////

  void Scene00001( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 1, HIDE_HOTBAR, bindSceneReturn( &ClsExc020::Scene00001Return ) );
  }

  void Scene00001Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
    quest.setSeq( Seq1 );
  }

  //////////////////////////////////////////////////////////////////////

  void Scene00002( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 2, HIDE_HOTBAR, bindSceneReturn( &ClsExc020::Scene00002Return ) );
  }

  void Scene00002Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
  }

  //////////////////////////////////////////////////////////////////////

  void Scene00003( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 3, FADE_OUT | HIDE_UI, bindSceneReturn( &ClsExc020::Scene00003Return ) );
  }

  void Scene00003Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
    if( result.errorCode == 0 || ( result.numOfResults > 0 && result.getResult( 0 ) == 1 ) )
    {
      quest.setSeq ( Seq2 );
    }
  }

  //////////////////////////////////////////////////////////////////////

  void Scene00004( World::Quest& quest, Entity::Player& player )
  {
    eventMgr().playQuestScene( player, getId(), 4, HIDE_HOTBAR, bindSceneReturn( &ClsExc020::Scene00004Return ) );
  }

  void Scene00004Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
  {
    if( result.getResult( 0 ) == 1 )
    {
      player.setRewardFlag( Sapphire::Common::UnlockEntry::HuntingLog );
      player.finishQuest( getId(), result.getResult( 1 ) );
    }
  }
};

EXPOSE_SCRIPT( ClsExc020 );