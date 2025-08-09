#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "action.h"
#include <any>
#include <bitset>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Character;
class Player;
class Enemy;
class Game;
class TargetingComponent;

using ExtraAttributes = std::vector<std::string>;
const int MAX = 2147483647;

enum class ConditionLogic
{
    AND,
    OR
};

// For the mode of the target selection
enum class TargetSelectionMode
{
    AUTOMATIC,
    MANUAL
};

// For the scope of the target selection
enum class TargetScope
{
    SINGLE,
    MULTI,
    ALL,
    NONE
};

// For group target selection
enum class TargetFaction
{
    ENEMIES,
    ALLIES,
    BOTH,
    NONE
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
    MISC
};

// Various conditional flags
enum class ConditionalType
{

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
    WasCriticalHit,
    SpellFailed,
    COUNT
};

enum class TargetCondition : size_t
{
    Stunned,
    Poisoned,
    Invisible,
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

struct Observer
{
    std::string actionType;
    std::string name;
    Character& caster;
    std::vector<Character*>& enemies;
    std::vector<Character*>& allies;
    std::vector<Character*> currentTargets;
    TargetScope scope;
    ConditionContainer states;
    std::string failureReason;
    int targetsDefeated = 0;

    Observer(Character& c, Game& game);
};

class DynamicValue
{
  public:
    int expression;

    void calculate(Observer& context);
};

bool evaluate(const Observer& context, const ConditionalType& condition);
bool evaluate(const Observer& context, const GameCondition& condition);
bool evaluate(const Observer& context, const TargetCondition& condition);
bool evaluateTargets(Observer& context, TargetingComponent& targetingComponent);
bool checkTargetHasCondition(const GameCondition& condition, Character* target);
bool checkTargetHasCondition(const TargetCondition& condition, Character* target);

class Components
{
  public:
    virtual ~Components() = default;
    virtual void execute(Observer& context) = 0;
    virtual bool shouldExecute(const Observer& context) const = 0;
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
    void execute(Observer& context) override;
};

class EffectComponent : public Components
{
  public:
    class PrimaryEffect
    {
      public:
        EffectType type;
        std::string subType;
        DynamicValue primaryValue;
        DynamicValue secondaryValue;
        ExtraAttributes extras;
    };

    class ConditionalEffect
    {
      public:
        PrimaryEffect primary;
        std::vector<GameCondition> gameConditions;
        std::vector<TargetCondition> targetConditions;
    };

    class DelayedEffect
    {
      public:
        PrimaryEffect primary;
        int turn;
    };

    std::vector<PrimaryEffect> primaryEffects;
    std::vector<ConditionalEffect> conditionalEffects;
    std::vector<DelayedEffect> delayedEffects;

    // Override base class methods
    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    void execute(Observer& context) override;
};

class UIComponent : public Components
{
  public:
    class PrimaryText
    {
      public:
        std::string text;
        bool typingMode;
        int time;
    };

    class ConditionalText
    {
      public:
        PrimaryText primary;
        std::vector<GameCondition> gameConditions;
        std::vector<TargetCondition> targetConditions;
    };

    std::vector<PrimaryText> primaryTexts;
    std::vector<ConditionalText> conditionalTexts;

    // Override base class methods
    ComponentCategory getCategory() const override;
    std::string getComponentType() const override;
    void execute(Observer& context) override;
};

#endif