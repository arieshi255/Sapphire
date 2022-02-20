#include "FateDirector.h"
#include "Director.h"

#include <Territory/InstanceContent.h>

#include <Network/PacketWrappers/EventLogMessagePacket.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include <Network/CommonActorControl.h>

#include "Actor/Player.h"

#include "Network/PacketWrappers/ActorControlPacket.h"
#include "Network/PacketWrappers/ActorControlSelfPacket.h"
#include <Logging/Logger.h>

#include <Service.h>
#include "WorldServer.h"
#include "Session.h"
#include "Network/GameConnection.h"

using namespace Sapphire::Common;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::WorldPackets::Server;
using namespace Sapphire::Network::ActorControl;

Sapphire::Event::FateDirector::FateDirector() : 
  Director( Event::Director::Fate, 0xFFFF )
{

}

Sapphire::Event::FateDirector::FateDirector( uint32_t fateId ) :
  Director( Event::Director::Fate, fateId )
{
  
}