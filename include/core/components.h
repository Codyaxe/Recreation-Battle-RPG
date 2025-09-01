#pragma once
#include "core/return_flags.h"
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
class Status;
class BattleContext;
class TargetingComponent;

// Forward declaration for Trait to avoid circular dependency
class Trait;

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
    STATUS,
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
    STATUS_STRENGTH,
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
    FAILED,
    FAILURE_TARGET_ALREADY_HAS_STATUS,
    FAILURE_TRAITS_CANNOT_STACK,
    FAILURE_ATTACK_ACTIVATION,
    FAILURE_BUFF_ACTIVATION,
    FAILURE_DEBUFF_ACTIVATION,
    FAILURE_ITEM_ACTIVATION,
    FAILURE_SPELL_ACTIVATION,
    FAILURE_STATUS_ACTIVATION,
    FAILURE_SUMMON_ACTIVATION,
    FAILURE_TRAIT_ACTIVATION,
    FAILURE_DEBUFF_EFFECT_DNE,
    FAILURE_BUFF_EFFECT_DNE,
    FAILURE_TRAIT_EFFECT_DNE,
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
    ON_DEATH, // Done
    ON_SUMMON,
    ON_INITIATE,        // Done
    ON_CAST,            // Done
    ON_AFFLICT,         // Done
    ON_BUFF,            // Done
    ON_EXHIBIT,         // Done
    ON_CALL,            // Done
    ON_USE,             // Done
    ON_CHANGE,          // TBA
    ON_PLAY,            // TBA
    ON_TARGET_INITIATE, // Done
    ON_TARGET_CAST,     // Done
    ON_TARGET_AFFLICT,  // Done
    ON_TARGET_BUFF,     // Done
    ON_TARGET_EXHIBIT,  // Done
    ON_TARGET_CALL,     // Done
    ON_TARGET_USE,      // Done
    ON_TARGET_CHANGE,   // TBA
    ON_TARGET_PLAY,     // TBA
    ON_DAMAGE_TAKEN,    // Done
    ON_DEALING_DAMAGE,  // Done
    ON_HEAL,            // Done
    ON_GAIN_X,          // Done
    ON_LOSE_X,          // Done
    ON_START_TURN,
    ON_END_TURN, // Placeholder
    ON_CRIT,
    ON_BLOCK,
    ON_PARRY,
    ON_COUNTER,
    ON_DODGE,
    ON_MISS,
    ON_KILL,          // Done
    ON_FATAL_DAMAGE,  // Done
    ON_STATUS_TICK,   // Done
    ON_STATUS_EXPIRE, // Done
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

// class StatusContainer
// {
//   public:
//     TargetCondition condition;
//     EventCondition triggerEvent; // When this status should activate
//     int duration;
//     int value; // Effect strength (damage, heal amount, stat boost, etc.)
//     bool isPermanent;

//     void reduce(int amount = 1);
//     void increase(int amount = 1);

//     bool shouldTriggerOn(EventCondition event) const;
//     bool trigger(Game& game, Character* character);
//     bool isExpired() const;

//     StatusContainer() = default;
//     StatusContainer(const TargetCondition& status, const int& duration);
//     StatusContainer(const TargetCondition& status, const int& duration, const int& effectValue);
//     StatusContainer(const TargetCondition& status, const int& duration, const int& effectValue,
//                     const EventCondition& trigger);
// };

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

    int calculate(BattleContext& context, DynamicValue& damage);
};

bool evaluate(const BattleContext& context, const GameCondition& condition);
bool evaluate(const BattleContext& context, const TargetCondition& condition);
bool checkTargetHasCondition(const GameCondition& condition, Character* target);
bool checkTargetHasCondition(const TargetCondition& condition, Character* target);
Return_Flags processTargets(BattleContext& context, TargetingComponent& targetingComponent);

class Component
{
  public:
    virtual ~Component() = default;
    virtual Return_Flags execute(BattleContext& context) = 0;
    virtual bool shouldExecute(const BattleContext& context) const;
    virtual ComponentCategory getCategory() const = 0;
    virtual std::string getComponentType() const = 0;

    ConditionLogic activateConditionLogic;
    std::vector<GameCondition> gameActivationConditions;
    std::vector<TargetCondition> targetActivationConditions;
    std::string componentName;
    int executionPriority = 0;
    bool isOptional = false;

    virtual bool canExecute(const BattleContext& context) const;
    virtual void onExecutionFailed(BattleContext& context, const std::string& reason);
};

class TargetingComponent : public Component
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
    Return_Flags execute(BattleContext& context) override;
};

class EffectComponent : public Component
{
  public:
    class PrimaryEffect
    {
      public:
        EffectType type;
        std::string subType;
        TargetCondition genericType;
        TraitCondition secondGenericType;
        std::unique_ptr<Status> statusType; // For Buffs, Debuffs
        std::unique_ptr<Trait> traitType;   // For Exhibit
        DynamicValue primaryValue;
        DynamicValue secondaryValue;
        ExtraAttributes extras;

        PrimaryEffect();
        ~PrimaryEffect(); // Destructor declaration for proper cleanup of unique_ptr<Trait>

        // Delete copy constructor and copy assignment operator
        PrimaryEffect(const PrimaryEffect&) = delete;
        PrimaryEffect& operator=(const PrimaryEffect&) = delete;

        // Declare move constructor and move assignment operator (defined in .cpp)
        PrimaryEffect(PrimaryEffect&&) noexcept;
        PrimaryEffect& operator=(PrimaryEffect&&) noexcept;

        PrimaryEffect(EffectType effectType, const std::string& subTypeName = "");
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      const DynamicValue& primary);
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      const DynamicValue& primary, const DynamicValue& secondary,
                      const ExtraAttributes& extraAttribs);
        // For Debuffs/Buffs
        PrimaryEffect(EffectType effectType, const TargetCondition& condition,
                      std::unique_ptr<Status>&& status);
        // Constructor that requires all attributes
        PrimaryEffect(EffectType effectType, const std::string& subTypeName,
                      std::unique_ptr<Status>&& status, std::unique_ptr<Trait>&& trait,
                      const DynamicValue& primary, const DynamicValue& secondary,
                      const ExtraAttributes& extraAttribs);
    };

    class ConditionalEffect
    {
      public:
        PrimaryEffect primary;
        std::vector<GameCondition> gameConditions;
        std::vector<TargetCondition> targetConditions;
        ConditionLogic targetingConditionLogic;

        ConditionalEffect();
        ConditionalEffect(PrimaryEffect&& effect);
        ConditionalEffect(PrimaryEffect&& effect, const std::vector<GameCondition>& gameReqs,
                          const std::vector<TargetCondition>& targetReqs,
                          const ConditionLogic& conditionLogic);
    };

    class DelayedEffect
    {
      public:
        PrimaryEffect primary;
        int turn;

        DelayedEffect();
        DelayedEffect(PrimaryEffect&& effect, int delayTurns);
    };

    std::vector<PrimaryEffect> primaryEffects;
    std::vector<ConditionalEffect> conditionalEffects;
    std::vector<DelayedEffect> delayedEffects;

    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    Return_Flags execute(BattleContext& context) override;
};

Return_Flags resolvePrimary(BattleContext& context, EffectComponent::PrimaryEffect& effect);
Return_Flags resolveConditional(BattleContext& context, EffectComponent::ConditionalEffect& effect);
Return_Flags resolveDelayed(BattleContext& context, EffectComponent::DelayedEffect& effect);

class MessageComponent : public Component
{
  private:
    bool processMessage(BattleContext& context, std::atomic<bool>& hasProceeded,
                        std::atomic<bool>& hasReachedEnd);

  public:
    class PrimaryText
    {
      public:
        std::string text;
        bool typingMode;
        bool isStatus = false;
        int delay;

        PrimaryText();
        PrimaryText(const std::string& textContent);
        PrimaryText(const std::string& textContent, bool enableTyping, int delayTime);
        PrimaryText(const std::string& textContent, bool enableTyping, bool enableStatusSettings,
                    int delayTime);
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
    Return_Flags execute(BattleContext& context) override;
};
