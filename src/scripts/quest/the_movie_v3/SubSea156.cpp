// FFXIVTheMovie.ParserV3.10
#include <Actor/Player.h>
#include <Actor/BNpc.h>
#include <ScriptObject.h>
#include <Service.h>
#include "Manager/TerritoryMgr.h"
#include "Manager/EventMgr.h"
#include "Territory/Territory.h"
#include <Manager/WarpMgr.h>

using namespace Sapphire;

class SubSea156 : public Sapphire::ScriptAPI::QuestScript
{
public:
  SubSea156() : Sapphire::ScriptAPI::QuestScript( 66825 ){}; 
  ~SubSea156() = default; 

  //SEQ_0, 1 entries
  //SEQ_1, 2 entries
  //SEQ_2, 4 entries
  //SEQ_3, 1 entries
  //SEQ_255, 1 entries

  //ACTOR0 = 1005394
  //ACTOR1 = 1005392
  //ACTOR2 = 1005393
  //ENEMY0 = 4507187
  //EOBJECT0 = 2002862
  //EVENTRANGE0 = 4497406
  //EVENTACTIONSEARCH = 1

  static constexpr auto Enemy0 = 4507187;

  static constexpr auto EVENT_ON_TALK = 0;
  static constexpr auto EVENT_ON_EMOTE = 1;
  static constexpr auto EVENT_ON_BNPC_KILL = 2;
  static constexpr auto EVENT_ON_WITHIN_RANGE = 3;
  static constexpr auto EVENT_ON_ENTER_TERRITORY = 4;
  static constexpr auto EVENT_ON_EVENT_ITEM = 5;
  static constexpr auto EVENT_ON_EOBJ_HIT = 6;
  static constexpr auto EVENT_ON_SAY = 7;

private:
  void onProgress( World::Quest& quest, Entity::Player& player, uint32_t type, uint64_t param1, uint32_t param2, uint32_t param3 )
  {
    switch( quest.getSeq() )
    {
      case 0:
      {
        if( type != EVENT_ON_BNPC_KILL ) Scene00000( quest, player ); // Scene00000: Normal(QuestOffer, TargetCanMove), id=unknown
        // +Callback Scene00001: Normal(Talk, QuestAccept, TargetCanMove), id=SKRIBYLD
        break;
      }
      case 1:
      {
        if( param1 == 1005392 ) // ACTOR1 = RKONTAI
        {
          if( quest.getUI8AL() != 1 )
          {
            Scene00002( quest, player ); // Scene00002: Normal(Talk, NpcDespawn, TargetCanMove), id=RKONTAI
          }
          break;
        }
        if( param1 == 1005394 ) // ACTOR0 = SKRIBYLD
        {
          Scene00003( quest, player ); // Scene00003: Normal(Talk, TargetCanMove), id=SKRIBYLD
          break;
        }
        break;
      }
      case 2:
      {
        if( param1 == 4497406 ) // EVENTRANGE0 = unknown
        {
          Scene00004( quest, player ); // Scene00004: Normal(Message, PopBNpc), id=unknown
          break;
        }
        if( param1 == 4507187 ) // ENEMY0 = unknown
        {
          quest.setUI8BH( 1 );
          eventMgr().sendEventNotice( player, getId(), 1, 0, 0, 0 );
          checkProgressSeq2( quest, player );
          break;
        }
        if( param1 == 1005393 ) // ACTOR2 = RKONTAI
        {
          Scene00005( quest, player ); // Scene00005: Normal(Talk, TargetCanMove), id=RKONTAI
          break;
        }
        if( param1 == 2002862 ) // EOBJECT0 = unknown
        {
          Scene00007( quest, player ); // Scene00007: Empty(None), id=unknown
          break;
        }
        break;
      }
      case 3:
      {
        if( type != EVENT_ON_BNPC_KILL ) Scene00008( quest, player ); // Scene00008: Normal(Talk, NpcDespawn, TargetCanMove), id=RKONTAI
        break;
      }
      case 255:
      {
        if( type != EVENT_ON_BNPC_KILL ) Scene00009( quest, player ); // Scene00009: Normal(Talk, QuestReward, QuestComplete, TargetCanMove), id=SKRIBYLD
        break;
      }
      default:
      {
        playerMgr().sendUrgent( player, "Sequence {} not defined.", quest.getSeq() );
        break;
      }
    }
  }

public:
  void onTalk( World::Quest& quest, Entity::Player& player, uint64_t actorId ) override
  {
    onProgress( quest, player, EVENT_ON_TALK, actorId, 0, 0 );
  }

  void onEmote( World::Quest& quest, uint64_t actorId, uint32_t emoteId, Sapphire::Entity::Player& player ) override
  {
    playerMgr().sendDebug( player, "emote: {}", emoteId );
    onProgress( quest, player, EVENT_ON_EMOTE, actorId, 0, emoteId );
  }

  void onBNpcKill( World::Quest& quest, Sapphire::Entity::BNpc& bnpc, Sapphire::Entity::Player& player ) override
  {
    onProgress( quest, player, EVENT_ON_BNPC_KILL, static_cast< uint64_t >( bnpc.getBNpcNameId() ), bnpc.getLayoutId(), 0 );
  }

  void onWithinRange( World::Quest& quest, Sapphire::Entity::Player& player, uint32_t eventId, uint32_t param1, float x, float y, float z ) override
  {
    onProgress( quest, player, EVENT_ON_WITHIN_RANGE, static_cast< uint64_t >( param1 ), 0, 0 );
  }

  void onEnterTerritory( World::Quest& quest, Sapphire::Entity::Player& player, uint16_t param1, uint16_t param2 ) override
  {
    onProgress( quest, player, EVENT_ON_ENTER_TERRITORY, static_cast< uint64_t >( param1 ), static_cast< uint32_t >( param2 ), 0 );
  }
  void onEventItem( World::Quest& quest, Sapphire::Entity::Player& player, uint64_t actorId ) override
  {
    onProgress( quest, player, EVENT_ON_EVENT_ITEM, actorId, 0, 0 );
  }
  void onEObjHit( World::Quest& quest, Sapphire::Entity::Player& player, uint64_t actorId, uint32_t actionId ) override
  {
    onProgress( quest, player, EVENT_ON_EOBJ_HIT, actorId, actionId, 0 );
  }
  void onSay( World::Quest& quest, Sapphire::Entity::Player& player, uint64_t actorId, uint32_t sayId ) override
  {
    onProgress( quest, player, EVENT_ON_SAY, actorId, sayId, 0 );
  }

private:
  void checkProgressSeq0( World::Quest& quest, Entity::Player& player )
  {
    quest.setSeq( 1 );
  }
  void checkProgressSeq1( World::Quest& quest, Entity::Player& player )
  {
    if( quest.getUI8AL() == 1 )
    {
      quest.setUI8AL( 0 );
      quest.setBitFlag8( 1, false );
      quest.setSeq( 2 );
    }
  }
  void checkProgressSeq2( World::Quest& quest, Entity::Player& player )
  {
    if( quest.getUI8BH() == 1 )
    {
      quest.setUI8BH( 0 );
      quest.setBitFlag8( 1, false );
      quest.setSeq( 3 );
    }
  }
  void checkProgressSeq3( World::Quest& quest, Entity::Player& player )
  {
    quest.setSeq( 255 );
  }

  void Scene00000( World::Quest& quest, Entity::Player& player ) //SEQ_0: , <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00000: Normal(QuestOffer, TargetCanMove), id=unknown" );
    playerMgr().sendDebug( player, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa" );

    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      if( result.numOfResults > 0 && result.getResult( 0 ) == 1 )
      {
        Scene00001( quest, player );
      }
    };
    eventMgr().playQuestScene( player, getId(), 0, HIDE_HOTBAR, callback );
  }
  void Scene00001( World::Quest& quest, Entity::Player& player ) //SEQ_0: , <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00001: Normal(Talk, QuestAccept, TargetCanMove), id=SKRIBYLD" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      checkProgressSeq0( quest, player );
    };
    eventMgr().playQuestScene( player, getId(), 1, HIDE_HOTBAR, callback );
  }

  void Scene00002( World::Quest& quest, Entity::Player& player ) //SEQ_1: ACTOR1, UI8AL = 1, Flag8(1)=True(Todo:0)
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00002: Normal(Talk, NpcDespawn, TargetCanMove), id=RKONTAI" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      quest.setUI8AL( 1 );
      quest.setBitFlag8( 1, true );
      eventMgr().sendEventNotice( player, getId(), 0, 0, 0, 0 );
      checkProgressSeq1( quest, player );
    };
    eventMgr().playQuestScene( player, getId(), 2, HIDE_HOTBAR, callback );
  }

  void Scene00003( World::Quest& quest, Entity::Player& player ) //SEQ_1: ACTOR0, <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00003: Normal(Talk, TargetCanMove), id=SKRIBYLD" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
    };
    eventMgr().playQuestScene( player, getId(), 3, HIDE_HOTBAR, callback );
  }

  void Scene00004( World::Quest& quest, Entity::Player& player ) //SEQ_2: EVENTRANGE0, <No Var>, Flag8(1)=True
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00004: Normal(Message, PopBNpc), id=unknown" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      quest.setBitFlag8( 1, true );
      auto instance = teriMgr().getTerritoryByGuId( player.getTerritoryId() );
      auto enemy = instance->createBNpcFromLayoutId( Enemy0, 120 /*Find the right value*/, Common::BNpcType::Enemy, player.getId() );
      enemy->hateListAdd( player.getAsPlayer(), 1 );
      checkProgressSeq2( quest, player );
    };
    eventMgr().playQuestScene( player, getId(), 4, HIDE_HOTBAR, callback );
  }


  void Scene00005( World::Quest& quest, Entity::Player& player ) //SEQ_2: ACTOR2, <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00005: Normal(Talk, TargetCanMove), id=RKONTAI" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
    };
    eventMgr().playQuestScene( player, getId(), 5, HIDE_HOTBAR, callback );
  }

  void Scene00007( World::Quest& quest, Entity::Player& player ) //SEQ_2: EOBJECT0, <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00007: Empty(None), id=unknown" );
    checkProgressSeq2( quest, player );
  }

  void Scene00008( World::Quest& quest, Entity::Player& player ) //SEQ_3: , <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00008: Normal(Talk, NpcDespawn, TargetCanMove), id=RKONTAI" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      checkProgressSeq3( quest, player );
    };
    eventMgr().playQuestScene( player, getId(), 8, HIDE_HOTBAR, callback );
  }

  void Scene00009( World::Quest& quest, Entity::Player& player ) //SEQ_255: , <No Var>, <No Flag>
  {
    playerMgr().sendDebug( player, "SubSea156:66825 calling Scene00009: Normal(Talk, QuestReward, QuestComplete, TargetCanMove), id=SKRIBYLD" );
    auto callback = [ & ]( World::Quest& quest, Entity::Player& player , const Event::SceneResult& result )
    {
      if( result.numOfResults > 0 && result.getResult( 0 ) == 1 )
      {
        player.finishQuest( getId(), result.getResult( 1 ) );
      }
    };
    eventMgr().playQuestScene( player, getId(), 9, HIDE_HOTBAR, callback );
  }
};

EXPOSE_SCRIPT( SubSea156 );
