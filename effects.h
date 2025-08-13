#ifndef EFFECTS_H
#define EFFECTS_H
#include "components.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class Character;

bool applyDamage(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyHeal(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyBuff(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyDebuff(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyExhibit(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applySummon(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyStats(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyMove(Observer& context, EffectComponent::PrimaryEffect& effect);
bool applyMisc(Observer& context, EffectComponent::PrimaryEffect& effect);

#endif