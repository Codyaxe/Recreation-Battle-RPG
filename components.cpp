#include "components.h"
#include "character.h"
#include "effects.h"
#include "observer.h"
#include "technical.h"
#include <algorithm>
#include <limits>
#include <random>
#include <thread>

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

int DynamicValue::calculate(BattleContext& context, DynamicValue& damage)
{
    // Base the damage on a particular stat.
    switch (damage.basis)
    {
    case DamageBasis::POWER:
        return static_cast<int>((context.source.power * damage.percentage) + damage.value);
    case DamageBasis::MAGIC:
        return static_cast<int>((context.source.magic * damage.percentage) + damage.value);
    case DamageBasis::HEALTH:
        return static_cast<int>((context.source.health * damage.percentage) + damage.value);
    case DamageBasis::DEFENSE:
        return static_cast<int>((context.source.defense * damage.percentage) + damage.value);
    case DamageBasis::SPEED:
        return static_cast<int>((context.source.speed * damage.percentage) + damage.value);
    case DamageBasis::ACCURACY:
        return static_cast<int>((context.source.accuracy * damage.percentage) + damage.value);
    case DamageBasis::MANA:
        return static_cast<int>((context.source.mana * damage.percentage) + damage.value);
    case DamageBasis::STATUS_STRENGTH:
        return static_cast<int>(context.statusStrength * damage.value);
    }
    return 0;
}

bool evaluate(const BattleContext& context, const GameCondition& condition)
{
    // GameCondition evaluation
    return false;
}

bool evaluate(const BattleContext& context, const TargetCondition& condition)
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

Return_Flags processTargets(BattleContext& context, TargetingComponent& targetingComponent)
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

    return Return_Flags::SUCCESS;
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
EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType,
                                              const TargetCondition& condition,
                                              std::unique_ptr<Status>&& status)
    : type(effectType), genericType(condition), statusType(std::move(status))
{
}

EffectComponent::PrimaryEffect::PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                                              std::unique_ptr<Status>&& status,
                                              std::unique_ptr<Trait>&& trait,
                                              const DynamicValue& primary,
                                              const DynamicValue& secondary,
                                              const ExtraAttributes& extraAttribs)
    : type(effectType), subType(subTypeName), statusType(std::move(status)),
      traitType(std::move(trait)), primaryValue(primary), secondaryValue(secondary),
      extras(extraAttribs)
{
}

// ConditionalEffect constructor implementations
EffectComponent::ConditionalEffect::ConditionalEffect() = default;

EffectComponent::ConditionalEffect::ConditionalEffect(PrimaryEffect&& effect)
    : primary(std::move(effect))
{
}

EffectComponent::ConditionalEffect::ConditionalEffect(
    PrimaryEffect&& effect, const std::vector<GameCondition>& gameReqs,
    const std::vector<TargetCondition>& targetReqs, const ConditionLogic& conditionLogic)
    : primary(std::move(effect)), gameConditions(gameReqs), targetConditions(targetReqs),
      targetingConditionLogic(conditionLogic)
{
}

// DelayedEffect constructor implementations
EffectComponent::DelayedEffect::DelayedEffect() : turn(0) {}

EffectComponent::DelayedEffect::DelayedEffect(PrimaryEffect&& effect, int delayTurns)
    : primary(std::move(effect)), turn(delayTurns)
{
}

// Component base class implementations
bool Component::canExecute(const BattleContext& context) const { return shouldExecute(context); }

void Component::onExecutionFailed(BattleContext& context, const std::string& reason)
{
    //
}

bool Component::shouldExecute(const BattleContext& context) const
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

Return_Flags TargetingComponent::execute(BattleContext& context)
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
        return Return_Flags::FAILED;
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

Return_Flags EffectComponent::execute(BattleContext& context)
{
    if (context.currentTargets.empty() && context.scope != TargetScope::NONE)
    {
        onExecutionFailed(context, "No targets available for effects");
        return Return_Flags::FAILED;
    }

    // A code that implements

    // Apply primary effects
    for (auto& effect : primaryEffects)
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
    return Return_Flags::SUCCESS;
}

Return_Flags resolvePrimary(BattleContext& context, EffectComponent::PrimaryEffect& effect)
{
    // Call function based on their effect type (eg. damage -> applyDamage(), heal -> applyHeal())
    switch (effect.type)
    {
    case EffectType::DAMAGE:
        return applyDamage(context, effect);
    case EffectType::HEAL:
        return applyHeal(context, effect);
    case EffectType::BUFF:
        return applyBuff(context, effect);
    case EffectType::DEBUFF:
        return applyDebuff(context, effect);
    case EffectType::EXHIBIT:
        return applyExhibit(context, effect);
    case EffectType::SUMMON:
        return Return_Flags::SUCCESS;
    case EffectType::STATS:
        return applyStats(context, effect);
    case EffectType::MOVE:
        return Return_Flags::SUCCESS;
    case EffectType::MISC:
        return Return_Flags::SUCCESS;
    }
    return Return_Flags::FAILED;
}

Return_Flags resolveConditional(BattleContext& context, EffectComponent::ConditionalEffect& effect)
{
    // For resolving effects with condition or none
    return resolvePrimary(context, effect.primary);
}

Return_Flags resolveDelayed(BattleContext& context, EffectComponent::DelayedEffect& effect)
{
    // For resolving effects with game condition or none
    return Return_Flags::SUCCESS;
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

MessageComponent::PrimaryText::PrimaryText(const std::string& textContent, bool enableTyping,
                                           bool enableStatusSettings, int delayTime)
    : text(textContent), typingMode(enableTyping), isStatus(enableStatusSettings), delay(delayTime)
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

bool MessageComponent::processMessage(BattleContext& context, std::atomic<bool>& hasProceeded,
                                      std::atomic<bool>& hasReachedEnd)
{
    // Start a thread to listen for the first skip (typing animation skip)
    std::mutex typingSkipMutex;
    std::condition_variable typingSkipCv;
    bool typingSkipPressed = false;
    bool typingSkipShouldExit = false;
    std::atomic<bool> typingPhaseComplete{false};

    std::thread typingSkipListener(
        [&]()
        {
            // Only listen for skips during the typing phase
            while (!typingPhaseComplete.load())
            {
                processSkip(typingSkipMutex, typingSkipCv, typingSkipPressed, typingSkipShouldExit);
                if (typingSkipPressed)
                {
                    hasProceeded.store(true);
                    break;
                }
            }
        });

    std::string storeSkippedString;
    if (!primaryTexts[0].isStatus)
    {
        clearScreen();
    }

    for (const auto& current : primaryTexts)
    {
        // I might do an in-place storing instead so I don't have to clear the screen. For now do
        // this instead.
        storeSkippedString += current.text + '\n';
        if (!hasProceeded.load())
        {
            if (current.typingMode && !current.isStatus)
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
                if (primary.typingMode && !primary.isStatus)
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

    // Process context dependent text
    // Will separate this into a new Generic Messaging Component to easily process battle
    // after-effects. (i.e damage dealt, effect applied, effect failed)
    std::string generic;
    for (auto& message : context.genericMessage)
    {
        if (!primaryTexts[0].isStatus) // Subject to change to be more readable
        {
            switch (message.type)
            {
            case EffectType::DAMAGE:
                generic = message.source->name + " have dealt " + std::to_string(message.amount) +
                          " to " + message.target->name + " using " +
                          static_cast<std::string>(message.name) + '\n';
                break;
            case EffectType::HEAL:
                generic = message.source->name + " healed " + message.target->name + " by " +
                          std::to_string(message.amount) + " using " +
                          static_cast<std::string>(message.name) + '\n';
                break;
            case EffectType::BUFF:
                generic = message.source->name + " have applied the buff " +
                          static_cast<std::string>(message.name) + " to target " +
                          message.target->name + '\n';
                break;
            case EffectType::DEBUFF:
                generic = message.source->name + " have applied the debuff " +
                          static_cast<std::string>(message.name) + " to target " +
                          message.target->name + '\n';
                break;
            case EffectType::EXHIBIT:
                generic = message.source->name + " have applied the trait " +
                          static_cast<std::string>(message.name) + " to target " +
                          message.target->name + '\n';
                break;
            case EffectType::SUMMON:
                generic = message.source->name + " have summoned " +
                          static_cast<std::string>(message.name) + '\n';
                break;
            }
        }
        else
        {
            switch (message.type)
            {
            case EffectType::DAMAGE:
                generic = message.source->name + " is dealt " + std::to_string(message.amount) +
                          " damage due to " + static_cast<std::string>(message.name) + '\n';
                break;
            }
        }
        storeSkippedString += generic + '\n';
        if (!hasProceeded.load())
        {
            if (primaryTexts[0].typingMode && !primaryTexts[0].isStatus)
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
            // If the typing mode is not turned on, output the generic message immediately
            else
            {
                std::cout << generic << '\n';
            }
        }
    }

    // for (int i = 0; i < context.currentTargets.size(); i++)
    // {
    //     if (context.actionType != ActionType::STATUS)
    //     {
    //         switch (context.effectType)
    //         {
    //         case EffectType::DAMAGE:
    //             generic = context.source.name + " have dealt " +
    //                       std::to_string(context.damageDealt[i]) + " to " +
    //                       context.currentTargets[i]->name + " using " + context.name + '\n';
    //         }
    //     }
    //     else
    //     {
    //         switch (context.effectType)
    //         {
    //         case EffectType::DAMAGE:
    //             generic = context.source.name + " is dealt " +
    //                       std::to_string(context.damageDealt[i]) + " damage due to " +
    //                       context.name + '\n';
    //         }
    //     }

    // If user have skipped, clear the screen and output all strings
    if (hasProceeded.load())
    {
        clearScreen();
        std::cout << storeSkippedString;
    }

    // Mark typing phase as complete
    typingPhaseComplete.store(true);
    {
        std::lock_guard<std::mutex> lock(typingSkipMutex);
        typingSkipShouldExit = true;
    }
    typingSkipCv.notify_one();
    typingSkipListener.join();

    if (!primaryTexts[0].isStatus)
    {
        std::cout << "Press Enter to continue...";
    }

    int dotCount = 0;
    const int maxDots = 3;
    int counter = 0;

    // For second skip
    std::mutex skipMutex;
    std::condition_variable skipCv;
    bool skipPressed = false;
    bool shouldExit = false;

    std::thread wait([&]() { processSkip(skipMutex, skipCv, skipPressed, shouldExit); });

    {
        std::unique_lock<std::mutex> lock(skipMutex);
        while (!skipPressed && !primaryTexts[0].isStatus)
        {
            if (skipCv.wait_for(lock, std::chrono::milliseconds(300),
                                [&skipPressed] { return skipPressed; }))
            {
                break;
            }

            // Update dots animation
            if (++counter % 2 == 0) // Delay for visual appeal
            {
                std::cout << '\r' << "Press Enter to continue   " << "\b\b\b";
                for (int i = 0; i < dotCount; ++i)
                {
                    std::cout << '.';
                }
                std::cout << std::flush;

                dotCount = (dotCount + 1) % (maxDots + 1);
            }
        }
        shouldExit = true;
    }

    wait.join();

    return true;
}

Return_Flags MessageComponent::execute(BattleContext& context)
{
    std::atomic<bool> hasSkipped{false};
    std::atomic<bool> hasReachedEnd{false};

    std::thread message(&MessageComponent::processMessage, this, std::ref(context),
                        std::ref(hasSkipped), std::ref(hasReachedEnd));

    message.join();

    return Return_Flags::SUCCESS;
}
