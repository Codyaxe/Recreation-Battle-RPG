#include "components.h"
#include "character.h"
#include "effects.h"
#include "observer.h"
#include "technical.h"
#include <algorithm>
#include <limits>
#include <random>
#include <thread>
StatusContainer::StatusContainer(const TargetCondition& status, const int& value)
    : condition(status), duration(value)
{
}

DynamicValue::DynamicValue() : value(100), percentage(1.0), basis(DamageBasis::POWER) {}

DynamicValue::DynamicValue(int fixedValue)
    : value(fixedValue), percentage(0.5), basis(DamageBasis::POWER)
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

int DynamicValue::calculate(Observer& context, DynamicValue& damage)
{
    // Base the damage on a particular stat.
    switch (damage.basis)
    {
    case DamageBasis::POWER:
        return static_cast<int>((context.caster.power * damage.percentage) + damage.value);
    case DamageBasis::MAGIC:
        return static_cast<int>((context.caster.magic * damage.percentage) + damage.value);
    case DamageBasis::HEALTH:
        return static_cast<int>((context.caster.health * damage.percentage) + damage.value);
    case DamageBasis::DEFENSE:
        return static_cast<int>((context.caster.defense * damage.percentage) + damage.value);
    case DamageBasis::SPEED:
        return static_cast<int>((context.caster.speed * damage.percentage) + damage.value);
    case DamageBasis::ACCURACY:
        return static_cast<int>((context.caster.accuracy * damage.percentage) + damage.value);
    case DamageBasis::MANA:
        return static_cast<int>((context.caster.mana * damage.percentage) + damage.value);
    }
    return 0;
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

bool processTargets(Observer& context, TargetingComponent& targetingComponent)
{
    // If the observer has current targets, due to spells having two targeting phases for two
    // effects, clear previous targets.
    if (!context.currentTargets.empty())
    {
        context.currentTargets.clear();
        context.damageDealt.clear();
    }

    std::vector<Character*> potentialTargets;
    std::vector<Character*> validTargets;

    // Find Potential Targets Depending on the Targeted Faction
    if (targetingComponent.faction == TargetFaction::ENEMIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        std::transform(context.enemies.begin(), context.enemies.end(),
                       std::back_inserter(potentialTargets),
                       [](const std::unique_ptr<Character>& c) { return c.get(); });
    }
    if (targetingComponent.faction == TargetFaction::ALLIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        std::transform(context.allies.begin(), context.allies.end(),
                       std::back_inserter(potentialTargets),
                       [](const std::unique_ptr<Character>& c) { return c.get(); });
    }
    // Find Valid Targets Depending on the Conditions
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

    // Randomly choose targets based on valid targets and the number of targets to select
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

EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                                              TargetCondition general, TraitCondition trait,
                                              const DynamicValue& primary,
                                              const DynamicValue& secondary,
                                              const ExtraAttributes& extraAttribs)
    : type(effectType), subType(subTypeName), genericType(general), traitType(trait),
      primaryValue(primary), secondaryValue(secondary), extras(extraAttribs)
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

// Components base class implementations
bool Components::canExecute(const Observer& context) const
{
    return shouldExecute(context) && !context.states.game.has(GameCondition::SPELL_FAILED);
}

void Components::onExecutionFailed(Observer& context, const std::string& reason)
{
    context.states.game.set(GameCondition::SPELL_FAILED);
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
    // Adjusts the numberofTarget variable depending on the target scope chosen
    if (scope == TargetScope::SINGLE)
    {
        numberOfTargets = 1;
    }
    else if (scope == TargetScope::ALL)
    {
        numberOfTargets = MAX;
    }
    // Validation for when the spell targers enemies and there are no valid enemies
    if (context.enemies.empty() && faction == TargetFaction::ENEMIES)
    {
        onExecutionFailed(context, "No enemies available to target");
        return true;
    }
    // Executes function depending on target selection mode
    if (mode == TargetSelectionMode::MANUAL)
    {
        return chooseTarget(context, *this);
    }
    else
    {
        return processTargets(context, *this);
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

    // A code that implements

    // Apply primary effects
    for (const auto& effect : primaryEffects)
    {
        return resolvePrimary(context, effect);
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

bool resolvePrimary(Observer& context, EffectComponent::PrimaryEffect effect)
{
    // Call function based on their effect type (eg. damage -> applyDamage(), heal -> applyHeal())
    switch (effect.type)
    {
    case EffectType::DAMAGE:
        return applyDamage(context, effect);
    case EffectType::HEAL:
        return true;
    case EffectType::BUFF:
        return true;
    case EffectType::DEBUFF:
        return true;
    case EffectType::EXHIBIT:
        return true;
    case EffectType::SUMMON:
        return true;
    case EffectType::STATS:
        return true;
    case EffectType::MOVE:
        return true;
    case EffectType::MISC:
        return true;
    }
    return false;
}

bool resolveConditional(Observer& context, EffectComponent::ConditionalEffect effect)
{
    // For resolving effects with condition or none
    return resolvePrimary(context, effect.primary);
}

bool resolveDelayed(Observer& context, EffectComponent::DelayedEffect effect)
{
    // For resolving effects with game condition or none
    return true;
}

// PrimaryText constructor implementations
MessageComponent::PrimaryText::PrimaryText() : text(""), typingMode(false), delay(0) {}

MessageComponent::PrimaryText::PrimaryText(const std::string& textContent)
    : text(textContent), typingMode(false), delay(0)
{
}

MessageComponent::PrimaryText::PrimaryText(const std::string& textContent, bool enableTyping,
                                           int delayTime)
    : text(textContent), typingMode(enableTyping), delay(delayTime)
{
}

MessageComponent::ConditionalText::ConditionalText() = default;

MessageComponent::ConditionalText::ConditionalText(const PrimaryText& effect) : primary(effect) {}

MessageComponent::ConditionalText::ConditionalText(const PrimaryText& text,
                                                   const std::vector<GameCondition>& gameReqs,
                                                   const std::vector<TargetCondition>& targetReqs,
                                                   const ConditionLogic& conditionLogic)
    : primary(text), gameConditions(gameReqs), targetConditions(targetReqs),
      targetingConditionLogic(conditionLogic)
{
}

// MessageComponent implementations
ComponentCategory MessageComponent::getCategory() const { return ComponentCategory::UI; }

std::string MessageComponent::getComponentType() const { return "MessageComponent"; }

bool MessageComponent::processMessage(Observer& context, std::atomic<bool>& hasProceeded,
                                      std::atomic<bool>& hasReachedEnd)
{
    std::string storeSkippedString;
    clearScreen();
    for (const auto& current : primaryTexts)
    {
        // I might do an in-place storing instead so I don't have to clear the screen. For now do
        // this instead.
        storeSkippedString += current.text + '\n';
        if (!hasProceeded.load())
        {
            if (current.typingMode)
            {
                const size_t batchSize = 3; // Batch based flushing
                const auto& text = current.text;
                for (size_t i = 0; i < text.size(); i += batchSize)
                {
                    if (!hasProceeded.load())
                    {
                        size_t end = std::min<int>(i + batchSize, text.size());
                        for (size_t j = i; j < end; ++j)
                        {
                            std::cout << text[j];
                        }

                        std::cout.flush();
                        Sleep(current.delay);
                    }
                    else
                    {

                        break;
                    }
                }
                if (!hasProceeded.load())
                {
                    std::cout << '\n';
                }
            }
            else
            {
                std::cout << current.text << '\n';
            }
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
            storeSkippedString += primary.text + '\n';
            if (!hasProceeded.load())
            {
                if (primary.typingMode)
                {
                    const size_t batchSize = 3;
                    for (size_t i = 0; i < primary.text.size(); i += batchSize)
                    {
                        if (!hasProceeded.load())
                        {
                            {
                                size_t end = std::min<size_t>(i + batchSize, primary.text.size());
                                for (size_t j = i; j < end; ++j)
                                {
                                    std::cout << primary.text[j];
                                }
                                std::cout.flush();
                                Sleep(primary.delay);
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (!hasProceeded.load())
                    {
                        std::cout << '\n';
                    }
                }
                else
                {
                    std::cout << primary.text << '\n';
                }
            }
        }
    }

    // Process Observer Dependent Text
    for (int i = 0; i < context.currentTargets.size(); i++)
    {
        std::string generic = "You have dealt " + std::to_string(context.damageDealt[i]) + " to " +
                              context.currentTargets[i]->name + " using " + context.name;
        storeSkippedString += generic + '\n';
        if (!hasProceeded.load())
        {
            if (primaryTexts[0].typingMode)
            {
                const size_t batchSize = 3;
                const auto text = generic;
                for (size_t i = 0; i < text.size(); i += batchSize)
                {
                    if (!hasProceeded.load())
                    {

                        {
                            size_t end = std::min<int>(i + batchSize, text.size());
                            for (size_t j = i; j < end; ++j)
                            {
                                std::cout << text[j];
                            }

                            std::cout.flush();
                            Sleep(primaryTexts[0].delay);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (!hasProceeded.load())
                {
                    std::cout << '\n';
                }
            }
            else
            {
                std::cout << generic << '\n';
            }
        }
    }

    if (hasProceeded.load())
    {
        clearScreen();
        std::cout << storeSkippedString;
    }
    hasReachedEnd.store(true);

    std::atomic<bool> hasProceededToNext{false};
    std::atomic<bool> hasProceededToNextEnd{false};

    std::thread wait(&MessageComponent::processSkip, this, std::ref(hasProceededToNext),
                     std::ref(hasProceededToNextEnd));

    std::cout << "Press Enter to continue...";

    int dotCount = 0;
    const int maxDots = 3;
    int counter = 0;

    while (!hasProceededToNext.load())
    {
        Sleep(50);

        if (++counter % 6 == 0)
        {
            std::cout << '\r' << "Press Enter to continue   " << "\b\b\b";
            for (int i = 0; i < dotCount; ++i)
                std::cout << '.';
            std::cout << std::flush;

            dotCount = (dotCount + 1) % (maxDots + 1);
        }
    }

    hasProceededToNextEnd.store(true);
    wait.join();

    return true;
}

bool MessageComponent::processSkip(std::atomic<bool>& hasProceeded,
                                   std::atomic<bool>& hasReachedEnd)
{
    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (!hasReachedEnd.load())
    {
        DWORD waitResult = WaitForSingleObject(Interface::hIn, 50);

        if (waitResult == WAIT_OBJECT_0)
        {
            if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
            {
                if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown &&
                    inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                {
                    hasProceeded.store(true);
                    break;
                }
            }
        }
    }
    return true;
}

bool MessageComponent::execute(Observer& context)
{
    std::atomic<bool> hasSkipped{false};
    std::atomic<bool> hasReachedEnd{false};
    std::thread message(&MessageComponent::processMessage, this, std::ref(context),
                        std::ref(hasSkipped), std::ref(hasReachedEnd));
    std::thread input(&MessageComponent::processSkip, this, std::ref(hasSkipped),
                      std::ref(hasReachedEnd));

    message.join();
    input.join();

    return true;
}
