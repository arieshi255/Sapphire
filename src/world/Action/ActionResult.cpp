#include "ActionResult.h"

#include <Util/Util.h>

#include <Service.h>
#include <Manager/PlayerMgr.h>

#include "Actor/Chara.h"
#include "Actor/Player.h"
#include "StatusEffect/StatusEffect.h"

#include "Network/Util/PacketUtil.h"

#include "Script/ScriptMgr.h"

using namespace Sapphire;
using namespace Sapphire::Common;
using namespace Sapphire::World::Action;


ActionResult::ActionResult( Entity::CharaPtr target ) :
  m_target( std::move( target ) )
{
  m_result.Arg0 = 0;
  m_result.Arg1 = 0;
  m_result.Arg2 = 0;
  m_result.Value = 0;
  m_result.Flag = static_cast< uint8_t >( ActionResultFlag::None );
  m_result.Type = CalcResultType::TypeNone;
}

Entity::CharaPtr ActionResult::getTarget() const
{
  return m_target;
}

void ActionResult::damage( uint32_t amount, CalcResultType hitType, uint8_t hitEffect, ActionResultFlag flag )
{
  m_result.Arg0 = hitEffect;
  m_result.Value = static_cast< int16_t >( amount );
  m_result.Flag = static_cast< uint8_t >( flag );
  m_result.Type = hitType;
}

void ActionResult::heal( uint32_t amount, CalcResultType hitType, uint8_t hitEffect, ActionResultFlag flag )
{
  m_result.Arg0 = hitEffect;
  m_result.Value = static_cast< int16_t >( amount );
  m_result.Flag = static_cast< uint8_t >( flag );
  m_result.Type = hitType;
}

void ActionResult::restoreMP( uint32_t amount, ActionResultFlag flag )
{
  m_result.Value = static_cast< int16_t >( amount );
  m_result.Flag = static_cast< uint8_t >( flag );
  m_result.Type = CalcResultType::TypeRecoverMp;
}

void ActionResult::startCombo( uint16_t actionId )
{
  m_result.Value = static_cast< int16_t >( actionId );
  m_result.Flag = static_cast< uint8_t >( ActionResultFlag::EffectOnSource );
  m_result.Type = CalcResultType::TypeCombo;
}

void ActionResult::comboSucceed()
{
  // no EffectOnSource flag on this
  m_result.Type = CalcResultType::TypeComboHit;
}

void ActionResult::applyStatusEffect( uint32_t id, int32_t duration, Entity::Chara& source, uint8_t param, bool shouldOverride )
{
  m_result.Value = static_cast< int16_t >( id );
  m_result.Arg2 = param;
  m_result.Type = CalcResultType::TypeSetStatus;

  m_bShouldOverride = shouldOverride;
  m_pStatus = Sapphire::StatusEffect::make_StatusEffect( id, source.getAsChara(), m_target, duration, 3000 );
  m_pStatus->setParam( param );
}

void ActionResult::applyStatusEffect( uint32_t id, int32_t duration, Entity::Chara& source, uint8_t param,
                                      const std::vector< StatusModifier >& modifiers, uint32_t flag, bool statusToSource, bool shouldOverride )
{
  m_result.Value = static_cast< int16_t >( id );
  m_result.Arg2 = param;
  m_result.Type = statusToSource ? CalcResultType::TypeSetStatusMe : CalcResultType::TypeSetStatus;
  if( statusToSource )
    m_result.Flag = static_cast< uint8_t >( ActionResultFlag::EffectOnSource );

  m_bShouldOverride = shouldOverride;
  m_pStatus = Sapphire::StatusEffect::make_StatusEffect( id, source.getAsChara(), m_target, duration, modifiers, flag, 3000 );
  m_pStatus->setParam( param );
}

void ActionResult::applyStatusEffectSelf( uint32_t id, int32_t duration, uint8_t param, bool shouldOverride )
{
  m_result.Value = static_cast< int16_t >( id );
  m_result.Arg2 = param;
  m_result.Type = CalcResultType::TypeSetStatusMe;
  m_result.Flag = static_cast< uint8_t >( ActionResultFlag::EffectOnSource );

  m_bShouldOverride = shouldOverride;
  m_pStatus = Sapphire::StatusEffect::make_StatusEffect( id, m_target, m_target, duration, 3000 );
  m_pStatus->setParam( param );
}

void ActionResult::applyStatusEffectSelf( uint32_t id, int32_t duration, uint8_t param, const std::vector< World::Action::StatusModifier >& modifiers,
                                          uint32_t flag, bool shouldOverride )
{
  m_result.Value = static_cast< int16_t >( id );
  m_result.Arg2 = param;
  m_result.Type = CalcResultType::TypeSetStatusMe;
  m_result.Flag = static_cast< uint8_t >( Common::ActionResultFlag::EffectOnSource );

  m_bShouldOverride = shouldOverride;
  m_pStatus = Sapphire::StatusEffect::make_StatusEffect( id, m_target, m_target, duration, modifiers, flag, 3000 );
  m_pStatus->setParam( param );
}

void ActionResult::replaceStatusEffect( Sapphire::StatusEffect::StatusEffectPtr& pOldStatus, uint32_t id, int32_t duration, Entity::Chara& source, uint8_t param,
                                        const std::vector< StatusModifier >& modifiers, uint32_t flag, bool statusToSource )
{
  applyStatusEffect( id, duration, source, param, modifiers, flag, statusToSource, false );
  m_pOldStatus = std::move( pOldStatus );
  m_pStatus->setSlot( m_pOldStatus->getSlot() );
}

void ActionResult::replaceStatusEffectSelf( Sapphire::StatusEffect::StatusEffectPtr& pOldStatus, uint32_t id, int32_t duration, uint8_t param,
                                            const std::vector< World::Action::StatusModifier >& modifiers, uint32_t flag )
{
  applyStatusEffectSelf( id, duration, param, modifiers, flag, false );
  m_pOldStatus = std::move( pOldStatus );
  m_pStatus->setSlot( m_pOldStatus->getSlot() );
}

void ActionResult::mount( uint16_t mountId )
{
  m_result.Value = static_cast< int16_t >( mountId );
  m_result.Arg0 = 1;
  m_result.Type = CalcResultType::TypeMount;
}

const Common::CalcResultParam& ActionResult::getCalcResultParam() const
{
  return m_result;
}

const Sapphire::StatusEffect::StatusEffectPtr ActionResult::getStatusEffect() const
{
  return m_pStatus;
}

void ActionResult::execute()
{
  if( !m_target )
    return;

  switch( m_result.Type )
  {
    case CalcResultType::TypeDamageHp:
    case CalcResultType::TypeCriticalDamageHp:
    {
      auto& scriptMgr = Common::Service< Scripting::ScriptMgr >::ref();
      auto statusEffects( m_target->getStatusEffectMap() );
      for( auto status : statusEffects )
      {
        scriptMgr.onPlayerHit( m_target, *status.second );
      }

      m_target->takeDamage( m_result.Value );
      break;
    }

    case CalcResultType::TypeRecoverHp:
    case CalcResultType::TypeCriticalRecoverHp:
    {
      m_target->heal( m_result.Value );
      break;
    }

    case CalcResultType::TypeRecoverMp:
    {
      m_target->restoreMP( m_result.Value );
      break;
    }

    case CalcResultType::TypeSetStatus:
    case CalcResultType::TypeSetStatusMe:
    {
      for( auto const& entry : m_target->getStatusEffectMap() )
      {
        auto statusEffect = entry.second;
        if( statusEffect->getId() == m_result.Value && m_bShouldOverride && statusEffect->getSrcActorId() == m_pStatus->getSrcActorId() )
        {
          statusEffect->refresh( m_pStatus->getDuration() );
          m_pStatus->setSlot( statusEffect->getSlot() );
          
          Network::Util::Packet::sendHudParam( *m_target );
          return;
        }
      }

      if( m_pOldStatus )
        m_target->replaceSingleStatusEffect( m_pOldStatus->getSlot(), m_pStatus );
      else if( !m_bShouldOverride )
        m_target->addStatusEffectByIdIfNotExist( m_pStatus );
      else
        m_target->addStatusEffectById( m_pStatus );
      break;
    }

    case CalcResultType::TypeMount:
    {
      auto pPlayer = m_target->getAsPlayer();
      pPlayer->setMount( m_result.Value );
      break;
    }

    default:
      break;
  }
}
