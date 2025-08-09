#include "components.h"
#include "technical.h"
#include <algorithm>
#include <random>

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
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
    if (targetingComponent.gameConditions.empty() && targetingComponent.targetConditions.empty())
    {
        validTargets = potentialTargets;
    }
    else if (targetingComponent.targetingConditionLogic == ConditionLogic::AND)
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

// PrimaryEffect constructor implementations
EffectComponent::PrimaryEffect::PrimaryEffect() = default;

EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType, const std::string& subTypeName)
    : type(effectType), subType(subTypeName)
{
}

EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                                              const DynamicValue& primary)
    : type(effectType), subType(subTypeName), primaryValue(primary)
{
}

EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                                              const DynamicValue& primary,
                                              const DynamicValue& secondary,
                                              const ExtraAttributes& extraAttribs)
    : type(effectType), subType(subTypeName), primaryValue(primary), secondaryValue(secondary),
      extras(extraAttribs)
{
}

// ConditionalEffect constructor implementations
EffectComponent::ConditionalEffect::ConditionalEffect() = default;

EffectComponent::ConditionalEffect::ConditionalEffect(const PrimaryEffect& effect) : primary(effect)
{
}

EffectComponent::ConditionalEffect::ConditionalEffect(
    const PrimaryEffect& effect, const std::vector<GameCondition>& gameReqs,
    const std::vector<TargetCondition>& targetReqs, const ConditionLogic& conditionLogic)
    : primary(effect), gameConditions(gameReqs), targetConditions(targetReqs),
      targetingConditionLogic(conditionLogic)
{
}

// DelayedEffect constructor implementations
EffectComponent::DelayedEffect::DelayedEffect() : turn(0) {}

EffectComponent::DelayedEffect::DelayedEffect(const PrimaryEffect& effect, int delayTurns)
    : primary(effect), turn(delayTurns)
{
}

// DynamicValue constructor implementations
DynamicValue::DynamicValue() : value(100), percentage(1.0), basis(DamageBasis::POWER) {}

DynamicValue::DynamicValue(int fixedValue)
    : value(fixedValue), percentage(1.0), basis(DamageBasis::POWER)
{
}

DynamicValue::DynamicValue(int fixedValue, DamageBasis damageBasis)
    : value(fixedValue), basis(damageBasis)
{
}

DynamicValue::DynamicValue(int fixedValue, double percentageValue, DamageBasis damageBasis)
    : value(fixedValue), percentage(percentageValue), basis(damageBasis)
{
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

bool TargetingComponent::execute(Observer& context)
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
        return true;
    }
    if (mode == TargetSelectionMode::MANUAL)
    {
        return chooseTarget(context, *this);
    }
    else
    {
        return evaluateTargets(context, *this);
    }
}

// EffectComponent implementations
ComponentCategory EffectComponent::getCategory() const { return ComponentCategory::EFFECTS; }

std::string EffectComponent::getComponentType() const { return "EffectComponent"; }

bool EffectComponent::execute(Observer& context)
{
    if (context.currentTargets.empty() && context.scope != TargetScope::NONE)
    {
        onExecutionFailed(context, "No targets available for effects");
        return true;
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
    return true;
}

// PrimaryText constructor implementations
UIComponent::PrimaryText::PrimaryText() : text(""), typingMode(false), delay(0) {}

UIComponent::PrimaryText::PrimaryText(const std::string& textContent)
    : text(textContent), typingMode(false), delay(0)
{
}

UIComponent::PrimaryText::PrimaryText(const std::string& textContent, bool enableTyping,
                                      int delayTime)
    : text(textContent), typingMode(enableTyping), delay(delayTime)
{
}

UIComponent::ConditionalText::ConditionalText() = default;

UIComponent::ConditionalText::ConditionalText(const PrimaryText& effect) : primary(effect) {}

UIComponent::ConditionalText::ConditionalText(const PrimaryText& text,
                                              const std::vector<GameCondition>& gameReqs,
                                              const std::vector<TargetCondition>& targetReqs,
                                              const ConditionLogic& conditionLogic)
    : primary(text), gameConditions(gameReqs), targetConditions(targetReqs),
      targetingConditionLogic(conditionLogic)
{
}

// UIComponent implementations
ComponentCategory UIComponent::getCategory() const { return ComponentCategory::UI; }

std::string UIComponent::getComponentType() const { return "UIComponent"; }

bool UIComponent::execute(Observer& context)
{

    for (const auto& current : primaryTexts)
    {
        if (current.typingMode)
        {
            const size_t batchSize = 3; // Batch based flushing
            const auto& text = current.text;

            for (size_t i = 0; i < text.size(); i += batchSize)
            {
                size_t end = std::min<int>(i + batchSize, text.size());
                for (size_t j = i; j < end; ++j)
                {
                    std::cout << text[j];
                }

                std::cout.flush();
                Sleep(current.delay);
            }
            std::cout << '\n';
            Sleep(1000);
        }
        else
        {
            std::cout << current.text << '\n';
            Sleep(1000);
        }
    }

    // Display conditional texts based on spell state
    for (const auto& conditionalText : conditionalTexts)
    {
        bool shouldDisplay = false;

        if (conditionalText.gameConditions.empty() && conditionalText.targetConditions.empty())
        {
            shouldDisplay = true;
        }
        else if (conditionalText.targetingConditionLogic == ConditionLogic::AND)
        {
            shouldDisplay = true;
            for (const auto& condition : conditionalText.gameConditions)
            {
                if (!evaluate(context, condition))
                {
                    shouldDisplay = false;
                    break;
                }
            }
            if (shouldDisplay)
            {
                for (const auto& condition : conditionalText.targetConditions)
                {
                    if (!evaluate(context, condition))
                    {
                        shouldDisplay = false;
                        break;
                    }
                }
            }
        }
        else
        {
            for (const auto& condition : conditionalText.gameConditions)
            {
                if (evaluate(context, condition))
                {
                    shouldDisplay = true;
                    break;
                }
            }
            if (!shouldDisplay)
            {
                for (const auto& condition : conditionalText.targetConditions)
                {
                    if (evaluate(context, condition))
                    {
                        shouldDisplay = true;
                        break;
                    }
                }
            }
        }

        if (shouldDisplay)
        {
            const auto& primary = conditionalText.primary;
            if (primary.typingMode)
            {
                const size_t batchSize = 3; // Batch based flushing
                for (size_t i = 0; i < primary.text.size(); i += batchSize)
                {
                    size_t end = std::min<size_t>(i + batchSize, primary.text.size());
                    for (size_t j = i; j < end; ++j)
                    {
                        std::cout << primary.text[j];
                    }
                    std::cout.flush();
                    Sleep(primary.delay);
                }
                std::cout << '\n';
            }
            else
            {
                std::cout << primary.text << '\n';
            }
        }
    }
    return true;
}
