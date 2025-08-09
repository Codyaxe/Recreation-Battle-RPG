#include "components.h"
#include "technical.h"
#include <algorithm>
#include <random>

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
}

bool evaluate(const Observer& context, const ConditionalType& condition)
{
    // ConditionalType evaluation
    return false;
}

bool evaluate(const Observer& context, const GameCondition& condition)
{
    // GameCondition evaluation
    return false;
}

bool evaluate(const Observer& context, const TargetCondition& condition)
{
    // TargetCondition evaluation
    return false;
}

bool evaluateTargets(Observer& context, TargetingComponent& targetingComponent)
{
    std::vector<Character*> potentialTargets;
    std::vector<Character*> validTargets;

    if (targetingComponent.faction == TargetFaction::ENEMIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        potentialTargets.insert(potentialTargets.end(), context.enemies.begin(),
                                context.enemies.end());
    }
    if (targetingComponent.faction == TargetFaction::ALLIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        potentialTargets.insert(potentialTargets.end(), context.allies.begin(),
                                context.allies.end());
    }
    if (targetingComponent.targetingConditionLogic == ConditionLogic::AND)
    {
        for (const auto& target : potentialTargets)
        {
            bool allConditionsMet = true;

            for (const auto& condition : targetingComponent.gameConditions)
            {
                if (!checkTargetHasCondition(condition, target))
                {
                    allConditionsMet = false;
                    break;
                }
            }

            for (const auto& condition : targetingComponent.targetConditions)
            {
                if (!checkTargetHasCondition(condition, target))
                {
                    allConditionsMet = false;
                    break;
                }
            }

            if (allConditionsMet)
            {
                validTargets.push_back(target);
            }
        }
    }
    else
    {
        for (const auto& target : potentialTargets)
        {

            for (const auto& condition : targetingComponent.gameConditions)
            {

                if (checkTargetHasCondition(condition, target))
                {
                    validTargets.push_back(target);
                    break;
                }
            }

            for (const auto& condition : targetingComponent.targetConditions)
            {

                if (checkTargetHasCondition(condition, target))
                {
                    validTargets.push_back(target);
                    break;
                }
            }
        }
    }

    if (!validTargets.empty())
    {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(validTargets.begin(), validTargets.end(), g);

        int targetsToSelect = std::min<int>(static_cast<int>(validTargets.size()),
                                            targetingComponent.numberOfTargets);

        for (int i = 0; i < targetsToSelect; ++i)
        {
            context.currentTargets.push_back(validTargets[i]);
        }
    }

    return !context.currentTargets.empty();
}

bool checkTargetHasCondition(const GameCondition& condition, Character* target)
{
    // Checks Game Condition State
    return false;
}

bool checkTargetHasCondition(const TargetCondition& condition, Character* target)
{
    // Checks Target State
    return false;
}

// Components base class implementations
bool Components::canExecute(const Observer& context) const
{
    return shouldExecute(context) && !context.states.game.has(GameCondition::SpellFailed);
}

void Components::onExecutionFailed(Observer& context, const std::string& reason)
{
    context.states.game.set(GameCondition::SpellFailed);
    context.failureReason = reason;
}

bool Components::shouldExecute(const Observer& context) const
{
    if (gameActivationConditions.empty() && targetActivationConditions.empty())
        return true;

    if (activateConditionLogic == ConditionLogic::OR)
    {

        for (const auto& condition : gameActivationConditions)
        {
            if (evaluate(context, condition))
            {
                return true;
            }
        }

        for (const auto& condition : targetActivationConditions)
        {
            if (evaluate(context, condition))
            {
                return true;
            }
        }
        return false;
    }
    else
    {

        for (const auto& condition : gameActivationConditions)
        {
            if (!evaluate(context, condition))
            {
                return false;
            }
        }
        for (const auto& condition : targetActivationConditions)
        {
            if (!evaluate(context, condition))
            {
                return false;
            }
        }
        return true;
    }
}

// TargetingComponent implementations
ComponentCategory TargetingComponent::getCategory() const { return ComponentCategory::TARGETING; }

std::string TargetingComponent::getComponentType() const { return "TargetingComponent"; }

void TargetingComponent::execute(Observer& context)
{
    if (scope == TargetScope::SINGLE)
    {
        numberOfTargets = 1;
    }
    else if (scope == TargetScope::ALL)
    {
        numberOfTargets = MAX;
    }
    if (context.enemies.empty() && faction == TargetFaction::ENEMIES)
    {
        onExecutionFailed(context, "No enemies available to target");
        return;
    }
    if (mode == TargetSelectionMode::MANUAL)
    {
        chooseTarget(context.enemies, context.allies, context.name, numberOfTargets,
                     static_cast<int>(faction), SPELL);
    }
    else
    {
        evaluateTargets(context, *this);
    }
}

// EffectComponent implementations
ComponentCategory EffectComponent::getCategory() const { return ComponentCategory::EFFECTS; }

std::string EffectComponent::getComponentType() const { return "EffectComponent"; }

void EffectComponent::execute(Observer& context)
{
    if (context.currentTargets.empty() && context.scope != TargetScope::NONE)
    {
        onExecutionFailed(context, "No targets available for effects");
        return;
    }

    // Apply primary effects
    for (const auto& effect : primaryEffects)
    {
        // Placeholder
    }

    // Apply conditional effects
    for (const auto& conditionalEffect : conditionalEffects)
    {
        // Placeholder
    }

    // Handle delayed effects
    for (const auto& delayedEffect : delayedEffects)
    {
        // Placeholder
    }
}

// UIComponent implementations
ComponentCategory UIComponent::getCategory() const { return ComponentCategory::UI; }

std::string UIComponent::getComponentType() const { return "UIComponent"; }

void UIComponent::execute(Observer& context)
{

    for (const auto& text : primaryTexts)
    {
        // Text display logic will go here
    }

    // Display conditional texts based on spell state
    for (const auto& conditionalText : conditionalTexts)
    {
        // Check conditions and display appropriate text
        // e.g., "Critical hit!" or "Spell failed!"
    }
}
