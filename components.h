#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "action.h"
#include <any>
#include <bitset>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <atomic>
#include <vector>

class Character;
class Player;
class Enemy;
class Game;
class TargetingComponent;
class Observer;

using ExtraAttributes = std::vector<std::string>;
const int MAX = 2147483647;

enum class ConditionLogic
{
    AND,
    OR,
    COUNT
};

// For the mode of the target selection
enum class TargetSelectionMode
{
    AUTOMATIC,
    MANUAL,
    COUNT
};

// For the scope of the target selection
enum class TargetScope
{
    SINGLE,
    MULTI,
    ALL,
    NONE,
    COUNT
};

// For group target selection
enum class TargetFaction
{
    ENEMIES,
    ALLIES,
    BOTH,
    NONE,
    COUNT
};

// Flags to detemine the current action
enum class ActionType
{
    SPELL,
    ATTACK,
    SUMMON,
    ITEM,
    DEBUFF,
    BUFF,
    TRAIT,
    COUNT
};

// Various effect types
enum class EffectType
{
    DAMAGE,
    HEAL,
    BUFF,
    DEBUFF,
    EXHIBIT,
    SUMMON,
    STATS,
    MOVE,
    MISC,
    COUNT
};

enum class GenericType
{

};

enum class DamageBasis
{
    NONE,
    POWER,
    MAGIC,
    HEALTH,
    DEFENSE,
    SPEED,
    ACCURACY,
    MANA,
    COUNT
};

// Component execution phases
enum class ComponentCategory
{
    TARGETING,
    EFFECTS,
    UI
};

enum class GameCondition : size_t
{
    WAS_CRITICAL_HIT,
    SPELL_FAILED,
    COUNT
};

enum class TargetCondition : size_t
{
    NONE,
    DEAD,
    STUN,
    POISON,
    INVISIBLE,
    COUNT
};

enum class TraitCondition : size_t
{
    NONE,
    COUNT,
};

enum class EventCondition
{
    ON_DEATH,
    ON_SUMMON,
    ON_INITIATE,
    ON_CAST,
    ON_AFFLICT,
    ON_BUFF,
    ON_EXHIBIT,
    ON_CALL,
    ON_USE,
    ON_CHANGE,
    ON_PLAY,
    ON_TARGET_INITIATE,
    ON_TARGET_CAST,
    ON_TARGET_AFFLICT,
    ON_TARGET_BUFF,
    ON_TARGET_EXHIBIT,
    ON_TARGET_CALL,
    ON_TARGET_USE,
    ON_TARGET_CHANGE,
    ON_TARGET_PLAY,
    ON_DAMAGE_TAKEN,
    ON_DEALING_DAMAGE,
    ON_HEAL,
    ON_GAIN_X,
    ON_LOSE_X,
    ON_START_TURN,
    ON_END_TURN,
    ON_CRIT,
    ON_BLOCK,
    ON_PARRY,
    ON_COUNTER,
    ON_DODGE,
    ON_MISS,
    ON_KILL,
    ON_FATAL_DAMAGE,
    COUNT
};

template <typename Enum, size_t N = static_cast<size_t>(Enum::COUNT)> class BitsetWrapper
{
  private:
    std::bitset<N> bits;

  public:
    void set(Enum e) { bits.set(static_cast<size_t>(e)); }
    void clear(Enum e) { bits.reset(static_cast<size_t>(e)); }
    bool has(Enum e) const { return bits.test(static_cast<size_t>(e)); }
    void clearAll() { bits.reset(); }
};

class StatusContainer
{
  public:
    TargetCondition condition;
    int duration;

    void reduce();
    void increase();

    StatusContainer() = default;
    StatusContainer(const TargetCondition& status, const int& value);
};

class ConditionContainer
{
  public:
    BitsetWrapper<GameCondition> game;
    BitsetWrapper<TargetCondition> target;

    ConditionContainer()
    {
        game.clearAll();
        target.clearAll();
    }
};

class DynamicValue
{
  public:
    int value;
    double percentage;
    DamageBasis basis;

    DynamicValue();
    DynamicValue(int fixedValue);
    DynamicValue(int fixedValue, DamageBasis damageBasis);
    DynamicValue(int fixedValue, double percentageValue, DamageBasis damageBasis);

    int calculate(Observer& context, DynamicValue& damage);
};

bool evaluate(const Observer& context, const GameCondition& condition);
bool evaluate(const Observer& context, const TargetCondition& condition);
bool checkTargetHasCondition(const GameCondition& condition, Character* target);
bool checkTargetHasCondition(const TargetCondition& condition, Character* target);
bool processTargets(Observer& context, TargetingComponent& targetingComponent);

class Components
{
  public:
    virtual ~Components() = default;
    virtual bool execute(Observer& context) = 0;
    virtual bool shouldExecute(const Observer& context) const;
    virtual ComponentCategory getCategory() const = 0;
    virtual std::string getComponentType() const = 0;

    ConditionLogic activateConditionLogic;
    std::vector<GameCondition> gameActivationConditions;
    std::vector<TargetCondition> targetActivationConditions;
    std::string componentName;
    int executionPriority = 0;
    bool isOptional = false;

    virtual bool canExecute(const Observer& context) const;
    virtual void onExecutionFailed(Observer& context, const std::string& reason);
};

class TargetingComponent : public Components
{
  public:
    TargetSelectionMode mode;
    TargetScope scope;
    int numberOfTargets;
    TargetFaction faction;
    std::vector<GameCondition> gameConditions;
    std::vector<TargetCondition> targetConditions;
    ConditionLogic targetingConditionLogic;

    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    bool execute(Observer& context) override;
};

class EffectComponent : public Components
{
  public:
    class PrimaryEffect
    {
      public:
        EffectType type;
        std::string subType;
        TargetCondition genericType; // For Buffs, Debuffs
        TraitCondition traitType;    // For Exhibit
        DynamicValue primaryValue;
        DynamicValue secondaryValue;
        ExtraAttributes extras;

        PrimaryEffect();
        PrimaryEffect(EffectType effectType, const std::string& subTypeName = "");
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      const DynamicValue& primary);
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      const DynamicValue& primary, const DynamicValue& secondary,
                      const ExtraAttributes& extraAttribs);
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      TargetCondition general, TraitCondition trait, const DynamicValue& primary,
                      const DynamicValue& secondary, const ExtraAttributes& extraAttribs);
    };

    class ConditionalEffect
    {
      public:
        PrimaryEffect primary;
        std::vector<GameCondition> gameConditions;
        std::vector<TargetCondition> targetConditions;
        ConditionLogic targetingConditionLogic;

        ConditionalEffect();
        ConditionalEffect(const PrimaryEffect& effect);
        ConditionalEffect(const PrimaryEffect& effect, const std::vector<GameCondition>& gameReqs,
                          const std::vector<TargetCondition>& targetReqs,
                          const ConditionLogic& conditionLogic);
    };

    class DelayedEffect
    {
      public:
        PrimaryEffect primary;
        int turn;

        DelayedEffect();
        DelayedEffect(const PrimaryEffect& effect, int delayTurns);
    };

    std::vector<PrimaryEffect> primaryEffects;
    std::vector<ConditionalEffect> conditionalEffects;
    std::vector<DelayedEffect> delayedEffects;

    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    bool execute(Observer& context) override;
};

bool resolvePrimary(Observer& context, EffectComponent::PrimaryEffect effect);
bool resolveConditional(Observer& context, EffectComponent::ConditionalEffect effect);
bool resolveDelayed(Observer& context, EffectComponent::DelayedEffect effect);

class MessageComponent : public Components
{
  private:
    bool processMessage(Observer& context, std::atomic<bool>& hasProceeded,
                        std::atomic<bool>& hasReachedEnd);
    bool processSkip(std::atomic<bool>& hasProceeded, std::atomic<bool>& hasReachedEnd);

  public:
    class PrimaryText
    {
      public:
        std::string text;
        bool typingMode;
        int delay;

        PrimaryText();
        PrimaryText(const std::string& textContent);
        PrimaryText(const std::string& textContent, bool enableTyping, int delayTime);
    };

    class ConditionalText
    {
      public:
        PrimaryText primary;
        std::vector<GameCondition> gameConditions;
        std::vector<TargetCondition> targetConditions;
        ConditionLogic targetingConditionLogic;

        ConditionalText();
        ConditionalText(const PrimaryText& text);
        ConditionalText(const PrimaryText& text, const std::vector<GameCondition>& gameReqs,
                        const std::vector<TargetCondition>& targetReqs,
                        const ConditionLogic& conditionLogic);
    };

    std::vector<PrimaryText> primaryTexts;
    std::vector<ConditionalText> conditionalTexts;

    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    bool execute(Observer& context) override;
};

#endif