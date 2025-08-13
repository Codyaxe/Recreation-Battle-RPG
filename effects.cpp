#include "effects.h"
#include "status.h"
#include "character.h"
#include "observer.h"
#include "technical.h"
#include <unordered_map>

bool applyDamage(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    // Element weakness/effectiveness table - maps attacker element to target weaknesses
    // Format: {"AttackType", "EffectiveAgainst1,EffectiveAgainst2,EffectiveAgainst3"}
    std::unordered_map<std::string, std::vector<std::string>> effectivenessTable = {
        {"Normal", {"Physical", "Ground", "Paranormal", "Weapon", "Steel"}},
        {"Physical", {"Normal", "Tech", "Weapon", "Geom", "Bomb"}},
        {"Fire", {"Nature", "Ice", "Water", "Bio", "Steel"}},
        {"Water", {"Fire", "Ground", "Electric", "Rock", "Magma"}},
        {"Wind", {"Fate", "Electric", "Ground", "Rock", "Psychic"}},
        {"Nature", {"Fire", "Poison", "Ice", "Chem", "Wind"}},
        {"Poison", {"Water", "Nature", "Bio", "Blood", "Mutate"}},
        {"Electric", {"Water", "Tech", "Wind", "Steel", "Tech"}},
        {"Ground", {"Water", "Electric", "Wind", "Nature", "Magma"}},
        {"Psychic", {"Wind", "Breakdown", "Dark", "Mystic", "Radioactive"}},
        {"Rock", {"Electric", "Ground", "Wind", "Fire", "Steel"}},
        {"Ice", {"Ground", "Nature", "Rock", "Geom", "Shard"}},
        {"Bio", {"Radioactive", "Nature", "Poison", "Chem", "Mutate"}},
        {"Chem", {"Poison", "Bio", "Mutate", "Geom", "Plasma"}},
        {"Geom", {"Chem", "Crystal", "Physical", "Fate", "Order"}},
        {"Mutate", {"Dark", "Breakdown", "Plasma", "Holy", "Chaos"}},
        {"Paranormal", {"Psychic", "Dark", "Mystic", "Holy", "Aether"}},
        {"Chaos", {"Mutate", "Fate", "Order", "Breakdown", "Special"}},
        {"Dark", {"Psychic", "Fate", "Order", "Holy", "Light"}},
        {"Mystic", {"Poison", "Paranormal", "Holy", "Light", "Rune"}},
        {"Order", {"Luck", "Fire", "Chaos", "Logos", "Pathos"}},
        {"Holy", {"Paranormal", "Poison", "Mystic", "Light", "Radioactive"}},
        {"Light", {"Dark", "Mystic", "Paranormal", "Crystal", "Shard"}},
        {"Steel", {"Fire", "Electric", "Psychic", "Rock", "Normal"}},
        {"Plasma", {"Chem", "Water", "Tech", "Force", "Aether"}},
        {"Bomb", {"Physical", "Weapon", "Force", "Breakdown", "Special"}},
        {"Blood", {"Bio", "Dark", "Mutate", "Ethos", "Dream"}},
        {"Shard", {"Ice", "Chrono", "Magma", "Breakdown", "Special"}},
        {"Crystal", {"Ice", "Geom", "Holy", "Shard", "Magma"}},
        {"Breakdown", {"Chaos", "Psychic", "Bio", "Bomb", "Blood"}},
        {"Special", {"Radioactive", "Shard", "Bomb", "Almighty", "Rune"}},
        {"Weapon", {"Physical", "Special", "Normal", "Tech", "Bomb"}},
        {"Interdimensional", {"Chrono", "Blood", "Logos", "Ethos", "Pathos"}},
        {"Sound", {"Interdimensional", "Shard", "Rune", "Dream", "Crystal"}},
        {"Aether", {"Paranormal", "Plasma", "Blood", "Interdimensional", "Force"}},
        {"Force", {"Plasma", "Bomb", "Chrono", "Light", "Weapon"}},
        {"Logos", {"Chaos", "Luck", "Sound", "Ethos", "Dream"}},
        {"Ethos", {"Order", "Sound", "Logos", "Dream", "Chrono"}},
        {"Pathos", {"Blood", "Sound", "Almighty", "Physical", "Weapon"}},
        {"Almighty", {"Special", "Light", "Chem", "Aether", "Luck"}},
        {"Rune", {"Mystic", "Interdimensional", "Sound", "Force", "Ethos"}},
        {"Dream", {"Sound", "Logos", "Aether", "Pathos", "Rune"}},
        {"Magma", {"Crystal", "Rock", "Steel", "Geom", "Ice"}},
        {"Tech", {"Luck", "Plasma", "Normal", "Almighty", "Radioactive"}},
        {"Radioactive", {"Almighty", "Dream", "Magma", "Chrono", "Normal"}},
        {"Chrono", {"Aether", "Force", "Almighty", "Rune", "Crystal"}},
        {"Luck", {"Interdimensional", "Fate", "Ethos", "Pathos", "Order"}},
        {"Fate", {"Interdimensional", "Luck", "Logos", "Pathos", "Chaos"}}};

    int damage = effect.primaryValue.calculate(context, effect.primaryValue); // Default damage
    // Apply damage to each chosen targets
    for (auto& target : context.currentTargets)
    {
        if (std::any_of(effectivenessTable.at(effect.subType).begin(),
                        effectivenessTable.at(effect.subType).end(),
                        [&target](const std::string& weakness)
                        { return weakness == target->element; }))
        {
            damage =
                effect.primaryValue.calculate(context, effect.primaryValue) * 2; // Super effective
        }
        /*If Critical Hit Do This Implementation*/
        // Stores the damage dealt for UI/Utility/Conditional purposes
        // Add Defense, Speed, Accuracy Checking
        context.damageDealt.push_back(damage);
        target->health -= damage;
        if (target->health <= 0)
        {
            target->targetConditions.set(TargetCondition::DEAD);
        }
    }
    return true;
};

bool applyHeal(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    int heal = effect.primaryValue.calculate(context, effect.primaryValue);
    for (auto& target : context.currentTargets)
    {
        target->health = (std::min)(heal, target->baseHealth);
    }

    return true;
};

/*   EffectType type: Type of Effect (eg. Damage, Heal, Buff)
   std::string subType:  Type of subtype (eg. damage -> ElementType, buff/debuff -> Buff/DebuffType)
   GenericType genericType
   DynamicValue primaryValue;
   DynamicValue secondaryValue;
   ExtraAttributes extras; */

bool applyBuff(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        target->targetConditions.set(effect.genericType);
        target->statuses.push_back(std::move(effect.statusType));
    }
    return true;
};

bool applyDebuff(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {

        target->targetConditions.set(effect.genericType);
        target->statuses.push_back(std::move(effect.statusType));
    }
    return true;
};

bool applyExhibit(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        target->acquiredTraits.push_back(std::move(effect.traitType));
    }
    return true;
};

bool applySummon(Observer& context, EffectComponent::PrimaryEffect& effect) { return true; };

bool applyStats(Observer& context, EffectComponent::PrimaryEffect& effect)
{
    std::unordered_map<std::string,
                       std::function<void(Character*, EffectComponent::PrimaryEffect&)>>
        statsMapping{
            {"Health",
             [](Character* c, EffectComponent::PrimaryEffect& efx)
             {
                 c->baseHealth += efx.primaryValue.value;
                 c->health += efx.primaryValue.value;
             }},
            {"Defense", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->defense += efx.primaryValue.value; }},
            {"Power", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->power += efx.primaryValue.value; }},
            {"Magic", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->magic += efx.primaryValue.value; }},
            {"Speed", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->speed += efx.primaryValue.value; }},
            {"Accuracy", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->accuracy += efx.primaryValue.value; }},
            {"Base_Mana", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->baseMana += efx.primaryValue.value, c->mana += efx.primaryValue.value; }},
            {"Mana", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->mana = (std::min)(c->mana + efx.primaryValue.value, c->baseMana); }},
            {"Resistance", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->resistance += efx.primaryValue.value; }},
            {"Weakness", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->weakness += efx.primaryValue.value; }},
            {"Luck", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->luck += efx.primaryValue.value; }},
            {"Absorption", [](Character* c, EffectComponent::PrimaryEffect& efx)
             { c->absorption += efx.primaryValue.value; }},
            {"Barrier",
             [&context](Character* c, EffectComponent::PrimaryEffect& efx)
             {
                 auto* temp = dynamic_cast<Player*>(c);
                 if (temp)
                 {
                     temp->barrier += efx.primaryValue.value;
                 }
                 else
                 {
                     context.states.game.set(GameCondition::FAILED);
                     context.failureReason = "Target has no Barrier Attribute.";
                 }
             }},
            {"Shield",
             [&context](Character* c, EffectComponent::PrimaryEffect& efx)
             {
                 auto* temp = dynamic_cast<Player*>(c);
                 if (temp)
                 {
                     temp->shield += efx.primaryValue.value;
                 }
                 else
                 {
                     context.states.game.set(GameCondition::FAILED);
                     context.failureReason = "Target has no Shield Attribute.";
                 }
             }},
            {"Armor",
             [&context](Character* c, EffectComponent::PrimaryEffect& efx)
             {
                 auto* temp = dynamic_cast<Player*>(c);
                 if (temp)
                 {
                     temp->armor += efx.primaryValue.value;
                 }
                 else
                 {
                     context.states.game.set(GameCondition::FAILED);
                     context.failureReason = "Target has no Armor Attribute.";
                 }
             }},
        };

    // Handles stats mapping and if found apply the stats
    auto it = statsMapping.find(effect.subType);
    for (auto& target : context.currentTargets)
    {
        if (it != statsMapping.end())
        {
            it->second(target, effect);
        }
    }

    return true;
};

bool applyMove(Observer& context, EffectComponent::PrimaryEffect& effect) { return true; };

bool applyMisc(Observer& context, EffectComponent::PrimaryEffect& effect) { return true; };