#ifndef EFFECTS_H
#define EFFECTS_H
#include "components.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "return_flags.h"

class Character;

Return_Flags applyDamage(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyHeal(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyBuff(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyDebuff(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyExhibit(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeBuff(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeDebuff(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeExhibit(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applySummon(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyStats(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyMove(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyMisc(BattleContext& context, EffectComponent::PrimaryEffect& effect);
#endif