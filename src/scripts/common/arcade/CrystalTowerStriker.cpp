#include <ScriptObject.h>
#include <Actor/Player.h>

#include <Util/Util.h>

#include <Network/PacketWrappers/ActorControlSelfPacket.h>
#include <Network/CommonActorControl.h>

#include "WorldServer.h"

using namespace Sapphire;
using namespace Sapphire::Network::ActorControl;
using namespace Sapphire::Network::Packets::WorldPackets::Server;

class CrystalTowerStriker :
  public Sapphire::ScriptAPI::EventScript
{
  private:
    static constexpr uint8_t Payout[3] = { 10, 15, 25 };
    static constexpr auto PrizeId = 9;
    uint8_t mgpEarned = 0;

  public:
    CrystalTowerStriker() : Sapphire::ScriptAPI::EventScript( 0x00240003 ){}; 
    ~CrystalTowerStriker() = default; 

  void onTalk( uint32_t eventId, Entity::Player& player, uint64_t actorId ) override
  {
    Scene00014( player );
  }

  void onYield( uint32_t eventId, uint16_t sceneId, uint8_t yieldId, Entity::Player& player, const std::string& resultString, uint64_t resultInt, const std::vector< uint32_t >& results ) override
  {
    auto& server = Common::Service< World::WorldServer >::ref();
    auto& exdData = Common::Service< Data::ExdData >::ref();
    auto arcadeInfo = exdData.getRow< Excel::GoldSaucerArcadeMachine >( getId() );

    switch( yieldId )
    {
      // start game
      case 7:
      {
        player.removeCurrency( Common::CurrencyType::Mgp, 1 );
        eventMgr().resumeScene( player, eventId, sceneId, 21, { 1, Common::Util::getTimeSeconds() + arcadeInfo->data().Time } );
        break;
      }
      // playing
      case 8:
      {
        auto score = arcadeInfo->data().Score[ results.at( 0 ) ];

        if( score.Score > 0 )
          mgpEarned = Payout[ score.Score - 1 ];
        else
          mgpEarned = 0;

        // TODO: move this somewhere better probably..
        server.queueForPlayer( player.getCharacterId(), makeActorControlSelf( player.getId(), DirectorUpdate, getId(), 1, mgpEarned ) );
        //eventMgr().resumeScene( player, eventId, sceneId, 22, { 1, 2, 15, 15, 121512, 1 } );
        eventMgr().resumeScene( player, eventId, sceneId, 22, { 1, score.Score, mgpEarned, mgpEarned, score.Icon, 1 } );
        break;
      }
    }
  }

  void Scene00014( Entity::Player& player )
  {
    auto callback = [ this ]( Entity::Player& player, const Event::SceneResult& result )
    {
      if( mgpEarned > 0 )
      {
        playerMgr().sendLogMessage( player, 4765, mgpEarned );
        playerMgr().sendPrizeMessage( player, PrizeId, mgpEarned );
        player.addCurrency( Common::CurrencyType::Mgp, mgpEarned );
      }
    };

    eventMgr().playScene( player, getId(), 14, HIDE_HOTBAR | NO_DEFAULT_CAMERA, callback );
  }
};

EXPOSE_SCRIPT( CrystalTowerStriker );