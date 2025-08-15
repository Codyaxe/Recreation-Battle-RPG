#ifndef EFFECTS_H
#define EFFECTS_H
#include "components.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "return_flags.h"

class Character;

Return_Flags applyDamage(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyHeal(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyBuff(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyDebuff(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyExhibit(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeBuff(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeDebuff(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags removeExhibit(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applySummon(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyStats(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyMove(Observer& context, EffectComponent::PrimaryEffect& effect);
Return_Flags applyMisc(Observer& context, EffectComponent::PrimaryEffect& effect);
#endif