// This is an automatically generated C++ script template
// Content needs to be added by hand to make it function
// In order for this script to be loaded, move it to the correct folder in <root>/scripts/

#include <Actor/Player.h>
#include <Actor/BNpc.h>
#include "Manager/EventMgr.h"
#include "Territory/Territory.h"
#include <ScriptObject.h>
#include <Service.h>

// Quest Script: SubSea054_00466
// Quest Name: Double Dealing
// Quest ID: 66002
// Start NPC: 1002626 (Staelwyrn)
// End NPC: 1002626 (Staelwyrn)

using namespace Sapphire;

class SubSea054 : public Sapphire::ScriptAPI::QuestScript
{
  private:
    // Basic quest information 
    // Quest vars / flags used
    // BitFlag8
    // UI8AL
    // UI8BH
    // UI8BL

    /// Countable Num: 1 Seq: 1 Event: 1 Listener: 1002640
    /// Countable Num: 3 Seq: 2 Event: 2 Listener: 1002640
    /// Countable Num: 1 Seq: 3 Event: 1 Listener: 2001279
    /// Countable Num: 1 Seq: 4 Event: 1 Listener: 1002641
    /// Countable Num: 1 Seq: 5 Event: 1 Listener: 1002642
    /// Countable Num: 1 Seq: 255 Event: 1 Listener: 1002643
    // Steps in this quest ( 0 is before accepting, 
    // 1 is first, 255 means ready for turning it in
    enum Sequence : uint8_t
    {
      Seq0 = 0,
      Seq1 = 1,
      Seq2 = 2,
      Seq3 = 3,
      Seq4 = 4,
      Seq5 = 5,
      SeqFinish = 255,
    };

    // Entities found in the script data of the quest
    static constexpr auto Actor0 = 1002626; // Staelwyrn ( Pos: 207.263000 112.839996 -222.431000  Teri: 134 )
    static constexpr auto Actor1 = 1002640; // Sevrin ( Pos: 131.089996 45.700699 134.447006  Teri: 134 )
    static constexpr auto Actor2 = 1002641; // Aylmer ( Pos: 186.636002 46.258099 124.655998  Teri: 134 )
    static constexpr auto Actor3 = 1002642; // Eyrimhus ( Pos: 191.895004 46.262901 128.084000  Teri: 134 )
    static constexpr auto Actor4 = 1002643; // Sozai Rarzai ( Pos: 194.462006 46.417000 119.236000  Teri: 134 )
    static constexpr auto Actor5 = 1002630; // Ossine ( Pos: 179.878998 64.612000 294.785004  Teri: 134 )
    static constexpr auto Enemy0 = 3931777; // 
    static constexpr auto Enemy1 = 3931779; // 
    static constexpr auto Eobject0 = 2001279; // Sack Of Oranges ( Pos: 132.807999 45.762199 134.938995  Teri: 134 )
    static constexpr auto Eobject1 = 2000451; // 
    static constexpr auto Eobject2 = 2000449; //  ( Pos: 132.807999 46.253899 134.938995  Teri: 134 )
    static constexpr auto Eventrange0 = 3929588;
    static constexpr auto EventAction = 35;
    static constexpr auto EventActionProcessMiddle = 16;
    static constexpr auto EventActionSearch = 1;
    static constexpr auto Item0 = 2000356;
    static constexpr auto Item1 = 2000357;
    static constexpr auto EmoteDoubt = 12;

    void checkProgressSeq1( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().sendEventNotice( player, getId(), 0 );
      if( quest.getUI8AL() == 1 )
      {
        quest.setUI8AL( 0 );
        quest.setBitFlag8( 1, false );
        quest.setSeq( Seq2 );
      }
    }

    void checkProgressSeq2( World::Quest& quest, Entity::Player& player, uint8_t index )
    {
      if( quest.getUI8AL() < 3 )
      {
        quest.setUI8AL( quest.getUI8AL() + 1 );
        quest.setBitFlag8( index, true );
        if( quest.getUI8AL() == 3 )
        {
          eventMgr().sendEventNotice( player, getId(), 1, 0 );
          quest.setUI8AL( 0 );
          quest.setBitFlag8( 1, false );
          quest.setSeq( Seq3 );
        }
        else
        {
          eventMgr().sendEventNotice( player, getId(), 1, 2, quest.getUI8AL(), 3 );
        }
      }
    }

    void checkProgressSeq3( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().sendEventNotice( player, getId(), 2, 0 );
      if( quest.getUI8AL() == 1 )
      {
        quest.setUI8AL( 0 );
        quest.setBitFlag8( 1, false );
        quest.setSeq( Seq4 );
      }
    }

    void checkProgressSeq4( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().sendEventNotice( player, getId(), 3, 0 );
      if( quest.getUI8AL() == 1 )
      {
        quest.setUI8AL( 0 );
        quest.setBitFlag8( 1, false );
        quest.setSeq( Seq5 );
        quest.setUI8BH( 1 );
      }
    }

    void checkProgressSeq5( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().sendEventNotice( player, getId(), 4, 0 );
      quest.setSeq( SeqFinish );
      quest.setUI8BH( 0 );
      quest.setUI8BL( 1 );
    }

  public:
    SubSea054() : Sapphire::ScriptAPI::QuestScript( 66002 ){}; 
    ~SubSea054() = default; 

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
        else if( quest.getSeq() == SeqFinish )
          Scene00036( quest, player );
        break;
      }
      case Actor1:
      {
        if( quest.getSeq() == Seq1 )
          Scene00001( quest, player );
        else if( quest.getSeq() == Seq3 )
          Scene00024( quest, player );
        break;
      }
      case Actor2:
      {
        if( quest.getSeq() == Seq2 && quest.getUI8AL() < 3 )
        {
          eventMgr().eventActionStart(player, getId(), EventAction,
            [ & ]( Entity::Player& player, uint32_t eventId, uint64_t additional ) {
              Scene00008( quest, player );
            }, nullptr, 0 );
        }
        break;
      }
      case Actor3:
      {
        if( quest.getSeq() == Seq2 && quest.getUI8AL() < 3 )
        {
          eventMgr().eventActionStart(player, getId(), EventAction,
            [ & ]( Entity::Player& player, uint32_t eventId, uint64_t additional ) {
              Scene00011( quest, player );
            }, nullptr, 0 );
        }
        break;
      }
      case Actor4:
      {
        if( quest.getSeq() == Seq2 && quest.getUI8AL() < 3 )
        {
          eventMgr().eventActionStart(player, getId(), EventAction,
            [ & ]( Entity::Player& player, uint32_t eventId, uint64_t additional ) {
              Scene00014( quest, player );
            }, nullptr, 0 );
        }
        break;
      }
      case Actor5:
      {
        if( quest.getSeq() == Seq5 )
          Scene00033( quest, player );
        break;
      }
      case Eobject0:
      {
        if( quest.getSeq() == Seq4 )
        {
          eventMgr().eventActionStart(player, getId(), EventActionProcessMiddle,
            [ & ]( Entity::Player& player, uint32_t eventId, uint64_t additional ) {
              Scene00028( quest, player );
            }, nullptr, 0 );
        }
      }
    }
  }

  void onEventItem( World::Quest& quest, Entity::Player& player, uint64_t actorId ) override
  {
  }

  void onWithinRange( World::Quest& quest, Sapphire::Entity::Player& player, uint32_t eventId, uint32_t param1, float x, float y, float z ) override
  {
    if( param1 == Eventrange0 )
    {
      auto instance = teriMgr().getTerritoryByGuId( player.getTerritoryId() );
      auto enemy0 = instance->getActiveBNpcByLayoutIdAndTriggerOwner( Enemy0, player.getId() );
      auto enemy1 = instance->getActiveBNpcByLayoutIdAndTriggerOwner( Enemy1, player.getId() );

      if( !enemy0 && !enemy1 )
        Scene00016( quest, player );
    }
  }

  void onEmote( World::Quest& quest, uint64_t actorId, uint32_t emoteId, Entity::Player& player ) override
  {
    if( quest.getSeq() == Seq1 && actorId == Actor1 )
    {
      if( emoteId == EmoteDoubt )
        Scene00002( quest, player );
    }
  }

  void onBNpcKill( World::Quest& quest, Entity::BNpc& bnpc, Entity::Player& player ) override
  {
    switch( bnpc.getBNpcNameId() )
    {
      case Enemy0: { break; }
      case Enemy1: { break; }
    }
  }

  private:
    //////////////////////////////////////////////////////////////////////
    // Available Scenes in this quest, not necessarly all are used
    //////////////////////////////////////////////////////////////////////

    void Scene00000( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 0, NONE, bindSceneReturn( &SubSea054::Scene00000Return ) );
    }

    void Scene00000Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      if( result.getResult( 0 ) == 1 ) // accept quest
      {
        quest.setSeq( Seq1 );
      }
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00001( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 1, NONE, bindSceneReturn( &SubSea054::Scene00001Return ) );
    }

    void Scene00001Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00002( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 2, NONE, bindSceneReturn( &SubSea054::Scene00002Return ) );
    }

    void Scene00002Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      quest.setUI8AL( 1 );
      quest.setBitFlag8( 1, true );
      checkProgressSeq1( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00003( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 3, NONE, bindSceneReturn( &SubSea054::Scene00003Return ) );
    }

    void Scene00003Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00004( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 4, NONE, bindSceneReturn( &SubSea054::Scene00004Return ) );
    }

    void Scene00004Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00005( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 5, NONE, bindSceneReturn( &SubSea054::Scene00005Return ) );
    }

    void Scene00005Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00006( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 6, NONE, bindSceneReturn( &SubSea054::Scene00006Return ) );
    }

    void Scene00006Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00007( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 7, NONE, bindSceneReturn( &SubSea054::Scene00007Return ) );
    }

    void Scene00007Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00008( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 8, NONE, bindSceneReturn( &SubSea054::Scene00008Return ) );
    }

    void Scene00008Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      checkProgressSeq2( quest, player, 1 );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00009( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 9, NONE, bindSceneReturn( &SubSea054::Scene00009Return ) );
    }

    void Scene00009Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00010( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 10, NONE, bindSceneReturn( &SubSea054::Scene00010Return ) );
    }

    void Scene00010Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00011( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 11, NONE, bindSceneReturn( &SubSea054::Scene00011Return ) );
    }

    void Scene00011Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      checkProgressSeq2( quest, player, 2 );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00012( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 12, NONE, bindSceneReturn( &SubSea054::Scene00012Return ) );
    }

    void Scene00012Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00013( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 13, NONE, bindSceneReturn( &SubSea054::Scene00013Return ) );
    }

    void Scene00013Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00014( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 14, NONE, bindSceneReturn( &SubSea054::Scene00014Return ) );
    }

    void Scene00014Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    { 
      checkProgressSeq2( quest, player, 3 );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00015( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 15, NONE, bindSceneReturn( &SubSea054::Scene00015Return ) );
    }

    void Scene00015Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00016( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 16, NONE, bindSceneReturn( &SubSea054::Scene00016Return ) );
    }

    void Scene00016Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      auto instance = teriMgr().getTerritoryByGuId( player.getTerritoryId() );
      auto enemy0 = instance->createBNpcFromLayoutId( Enemy0, 150 /*Find the right value*/, Common::BNpcType::Enemy, player.getId() );
      auto enemy1 = instance->createBNpcFromLayoutId( Enemy1, 150 /*Find the right value*/, Common::BNpcType::Enemy, player.getId() );
      enemy0->hateListAdd( player.getAsPlayer(), 1 );
      enemy1->hateListAdd( player.getAsPlayer(), 1 );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00017( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 17, NONE, bindSceneReturn( &SubSea054::Scene00017Return ) );
    }

    void Scene00017Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00018( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 18, NONE, bindSceneReturn( &SubSea054::Scene00018Return ) );
    }

    void Scene00018Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00019( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 19, NONE, bindSceneReturn( &SubSea054::Scene00019Return ) );
    }

    void Scene00019Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00020( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 20, NONE, bindSceneReturn( &SubSea054::Scene00020Return ) );
    }

    void Scene00020Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00021( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 21, NONE, bindSceneReturn( &SubSea054::Scene00021Return ) );
    }

    void Scene00021Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00022( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 22, NONE, bindSceneReturn( &SubSea054::Scene00022Return ) );
    }

    void Scene00022Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00023( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 23, NONE, bindSceneReturn( &SubSea054::Scene00023Return ) );
    }

    void Scene00023Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00024( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 24, NONE, bindSceneReturn( &SubSea054::Scene00024Return ) );
    }

    void Scene00024Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      quest.setUI8AL( 1 );
      quest.setBitFlag8( 1, true );
      checkProgressSeq3( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00025( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 25, NONE, bindSceneReturn( &SubSea054::Scene00025Return ) );
    }

    void Scene00025Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00026( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 26, NONE, bindSceneReturn( &SubSea054::Scene00026Return ) );
    }

    void Scene00026Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00027( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 27, NONE, bindSceneReturn( &SubSea054::Scene00027Return ) );
    }

    void Scene00027Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00028( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 28, NO_DEFAULT_CAMERA, bindSceneReturn( &SubSea054::Scene00028Return ) );
    }

    void Scene00028Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      Scene00029( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00029( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 29, NO_DEFAULT_CAMERA, bindSceneReturn( &SubSea054::Scene00029Return ) );
    }

    void Scene00029Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      quest.setUI8AL( 1 );
      quest.setBitFlag8( 1, true );
      checkProgressSeq4( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00030( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 30, NONE, bindSceneReturn( &SubSea054::Scene00030Return ) );
    }

    void Scene00030Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00031( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 31, NONE, bindSceneReturn( &SubSea054::Scene00031Return ) );
    }

    void Scene00031Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00032( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 32, NONE, bindSceneReturn( &SubSea054::Scene00032Return ) );
    }

    void Scene00032Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00033( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 33, NONE, bindSceneReturn( &SubSea054::Scene00033Return ) );
    }

    void Scene00033Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      if( result.getResult( 0 ) == 1 )
        Scene00034( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00034( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 34, NONE, bindSceneReturn( &SubSea054::Scene00034Return ) );
    }

    void Scene00034Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      checkProgressSeq5( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00035( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 35, NONE, bindSceneReturn( &SubSea054::Scene00035Return ) );
    }

    void Scene00035Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

    //////////////////////////////////////////////////////////////////////

    void Scene00036( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 36, NONE, bindSceneReturn( &SubSea054::Scene00036Return ) );
    }

    void Scene00036Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      if( result.getResult( 0 ) == 1 )
        Scene00037( quest, player );
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00037( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 37, NONE, bindSceneReturn( &SubSea054::Scene00037Return ) );
    }

    void Scene00037Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {
      if( result.getResult( 0 ) == 1 )
      {
        quest.setUI8BL( 0 );
        player.finishQuest( getId(), result.getResult( 1 ) );
      }
    }

    //////////////////////////////////////////////////////////////////////

    void Scene00038( World::Quest& quest, Entity::Player& player )
    {
      eventMgr().playQuestScene( player, getId(), 38, NONE, bindSceneReturn( &SubSea054::Scene00038Return ) );
    }

    void Scene00038Return( World::Quest& quest, Entity::Player& player, const Event::SceneResult& result )
    {


    }

};

EXPOSE_SCRIPT( SubSea054 );