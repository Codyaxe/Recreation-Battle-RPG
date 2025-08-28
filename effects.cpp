#include "effects.h"
#include "status.h"
#include "traits.h"
#include "character.h"
#include "observer.h"
#include "technical.h"
#include <unordered_map>
#include <random>

Return_Flags applyDamage(BattleContext& context, EffectComponent::PrimaryEffect& effect)
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

        std::random_device r;
        std::mt19937 gen(r());
        std::uniform_int_distribution<int> dist(0, 100);

        int value = dist(gen);

        if (value >= 20)
        {
            damage *= 2;
            EventData critEvent(EventCondition::ON_CRIT, &context.source);
            Interface::eventBattleContext.enqueue(critEvent);
            Interface::eventBattleContext.waitForEventProcessing();
        }

        // Stores the damage dealt for UI/Utility/Conditional purposes
        // Add Defense, Speed, Accuracy Checking

        // Dodge checking

        context.genericMessage.push_back(
            EffectMessage(EffectType::DAMAGE, context.name, damage, &context.source, target));
        target->health -= damage;

        EventData eventDealingDamage(EventCondition::ON_DEALING_DAMAGE, &context.source, damage);
        Interface::eventBattleContext.enqueue(eventDealingDamage);
        EventData eventDamageTaken(EventCondition::ON_DAMAGE_TAKEN, target, damage);
        Interface::eventBattleContext.enqueue(eventDamageTaken);
        Interface::eventBattleContext.waitForEventProcessing();

        if (target->health <= 0)
        {
            target->targetConditions.set(TargetCondition::DEAD);
            EventData eventDeath(EventCondition::ON_DEATH, target);
            Interface::eventBattleContext.enqueue(eventDeath);
            EventData eventFatal(EventCondition::ON_FATAL_DAMAGE, target);
            Interface::eventBattleContext.enqueue(eventFatal);
            EventData eventKill(EventCondition::ON_KILL, &context.source);
            Interface::eventBattleContext.enqueue(eventKill);
            Interface::eventBattleContext.waitForEventProcessing();
        }
    }
    return Return_Flags::SUCCESS;
};

Return_Flags applyHeal(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    int heal = effect.primaryValue.calculate(context, effect.primaryValue);
    for (auto& target : context.currentTargets)
    {
        // Cannot heal exceeding base health
        target->health = (std::min)(heal, target->baseHealth);
        EventData eventHeal(EventCondition::ON_HEAL, target, heal);
        Interface::eventBattleContext.enqueue(eventHeal);
        Interface::eventBattleContext.waitForEventProcessing();
        context.genericMessage.push_back(EffectMessage(EffectType::DAMAGE, context.name,
                                                       (std::min)(heal, target->baseHealth),
                                                       &context.source, target));
    }

    return Return_Flags::SUCCESS;
};

/*   EffectType type: Type of Effect (eg. Damage, Heal, Buff)
   std::string subType:  Type of subtype (eg. damage -> ElementType, buff/debuff -> Buff/DebuffType)
   GenericType genericType
   DynamicValue primaryValue;
   DynamicValue secondaryValue;
   ExtraAttributes extras; */

Return_Flags applyBuff(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        if (!target->targetConditions.has(effect.genericType))
        {
            target->targetConditions.set(effect.genericType);
            if (effect.statusType)
            {
                auto statusClone = effect.statusType->clone();
                EventData eventBuff(EventCondition::ON_GAIN_X, effect.genericType, target);

                Interface::eventBattleContext.enqueue(eventBuff);
                Interface::eventBattleContext.waitForEventProcessing();
                target->statuses.push_back(std::move(statusClone));

                context.genericMessage.push_back(
                    EffectMessage(EffectType::BUFF, context.name, &context.source, target));
            }
            else
            {
                EffectMessage message =
                    EffectMessage(EffectType::BUFF, context.name, &context.source, target);
                message.conditions.set(GameCondition::FAILURE_DEBUFF_EFFECT_DNE);
                context.genericMessage.push_back(message);
            }
        }
        else
        {
            // REAPPLY INSTEAD OF ERROR
            EffectMessage message =
                EffectMessage(EffectType::BUFF, context.name, &context.source, target);
            message.conditions.set(GameCondition::FAILURE_TARGET_ALREADY_HAS_STATUS);
            context.genericMessage.push_back(message);
        }
    }
    return Return_Flags::SUCCESS;
};

// DEBUFFS AND BUFF MIGHT HAVE DIFFERENT IMPLEMENTATION FOR NOW IGNORE THIS DUPLICATE CODE
Return_Flags applyDebuff(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        if (!target->targetConditions.has(effect.genericType))
        {
            target->targetConditions.set(effect.genericType);
            if (effect.statusType)
            {
                auto statusClone = effect.statusType->clone();
                target->statuses.push_back(std::move(statusClone));

                EventData eventDebuff(EventCondition::ON_GAIN_X, effect.genericType, target);
                Interface::eventBattleContext.enqueue(eventDebuff);
                Interface::eventBattleContext.waitForEventProcessing();

                context.genericMessage.push_back(
                    EffectMessage(EffectType::DEBUFF, context.name, &context.source, target));
            }
            else
            {
                EffectMessage message =
                    EffectMessage(EffectType::DEBUFF, context.name, &context.source, target);
                message.conditions.set(GameCondition::FAILURE_BUFF_EFFECT_DNE);
                context.genericMessage.push_back(message);
            }
        }
        else
        {
            // REAPPLY INSTEAD OF ERROR
            EffectMessage message =
                EffectMessage(EffectType::DEBUFF, context.name, &context.source, target);
            message.conditions.set(GameCondition::FAILURE_TARGET_ALREADY_HAS_STATUS);
            context.genericMessage.push_back(message);
        }
    }
    return Return_Flags::SUCCESS;
};

Return_Flags applyExhibit(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        if (!target->traitConditions.has(effect.secondGenericType))
        {
            target->traitConditions.set(effect.secondGenericType);
            if (effect.traitType)
            {
                auto traitClone = effect.traitType->clone();
                target->acquiredTraits.push_back(std::move(traitClone));

                EventData eventTrait(EventCondition::ON_GAIN_X, effect.secondGenericType, target);
                Interface::eventBattleContext.enqueue(eventTrait);
                Interface::eventBattleContext.waitForEventProcessing();

                context.genericMessage.push_back(
                    EffectMessage(EffectType::EXHIBIT, context.name, &context.source, target));
            }
        }
        else
        {
            EffectMessage message =
                EffectMessage(EffectType::BUFF, context.name, &context.source, target);
            message.conditions.set(GameCondition::FAILURE_TRAITS_CANNOT_STACK);
            context.genericMessage.push_back(message);
        }
    }
    return Return_Flags::SUCCESS;
};
// DEBUFFS AND BUFF MIGHT HAVE DIFFERENT IMPLEMENTATION FOR NOW IGNORE THIS DUPLICATE CODE
Return_Flags removeBuff(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        target->targetConditions.clear(effect.genericType);

        EventData eventLoseBuff(EventCondition::ON_LOSE_X, effect.genericType, target);
        Interface::eventBattleContext.enqueue(eventLoseBuff);
        Interface::eventBattleContext.waitForEventProcessing();

        auto& statuses = target->statuses;
        auto it = std::remove_if(statuses.begin(), statuses.end(),
                                 [&effect](const std::unique_ptr<Status>& status)
                                 { return status && (status.get() == effect.statusType.get()); });
        statuses.erase(it, statuses.end());
    }
    return Return_Flags::SUCCESS;
};
Return_Flags removeDebuff(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    for (auto& target : context.currentTargets)
    {
        target->targetConditions.clear(effect.genericType);

        EventData eventLoseDebuff(EventCondition::ON_LOSE_X, effect.genericType, target);
        Interface::eventBattleContext.enqueue(eventLoseDebuff);
        Interface::eventBattleContext.waitForEventProcessing();

        auto& statuses = target->statuses;
        auto it = std::remove_if(statuses.begin(), statuses.end(),
                                 [&effect](const std::unique_ptr<Status>& status)
                                 { return status && (status.get() == effect.statusType.get()); });
        statuses.erase(it, statuses.end());
    }
    return Return_Flags::SUCCESS;
};
Return_Flags removeExhibit(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{

    for (auto& target : context.currentTargets)
    {
        target->traitConditions.clear(effect.secondGenericType);

        EventData eventLoseTrait(EventCondition::ON_LOSE_X, effect.secondGenericType, target);
        Interface::eventBattleContext.enqueue(eventLoseTrait);
        Interface::eventBattleContext.waitForEventProcessing();

        auto& acquiredTraits = target->acquiredTraits;
        auto it = std::remove_if(acquiredTraits.begin(), acquiredTraits.end(),
                                 [&effect](const std::unique_ptr<Trait>& trait)
                                 { return trait && (trait.get() == effect.traitType.get()); });
        acquiredTraits.erase(it, acquiredTraits.end());
    }
    return Return_Flags::SUCCESS;
};

Return_Flags applySummon(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    return Return_Flags::SUCCESS;
};

Return_Flags applyStats(BattleContext& context, EffectComponent::PrimaryEffect& effect)
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
                     EffectMessage message =
                         EffectMessage(EffectType::STATS, context.name, &context.source, c);
                     message.conditions.set(GameCondition::FAILED);
                     context.genericMessage.push_back(message);
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
                     EffectMessage message =
                         EffectMessage(EffectType::STATS, context.name, &context.source, c);
                     message.conditions.set(GameCondition::FAILED);
                     context.genericMessage.push_back(message);
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
                     EffectMessage message =
                         EffectMessage(EffectType::STATS, context.name, &context.source, c);
                     message.conditions.set(GameCondition::FAILED);
                     context.genericMessage.push_back(message);
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

    return Return_Flags::SUCCESS;
};

Return_Flags applyMove(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    return Return_Flags::SUCCESS;
};

Return_Flags applyMisc(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    return Return_Flags::SUCCESS;
};