

// Handles UI, STATS, AND PERHAPS SOUND
// GameEventBattleContext interface
// GameEventSubject base class
// event structures (SpellCastEvent, etc.)
// concrete observers (UIBattleContext, StatsBattleContext, etc.)
// event notification system
// Handles graveyard
// Handle observers in effects
// Handles pending actions
// Handle if something is damaged, healed, obtain stats, lose stats, attain buff/debuffs, triggers
// x, and has x;
#ifndef OBSERVER_H
#define OBSERVER_H

#include "components.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <memory>
#include <functional>
#include <string_view>

using ObservedData = std::tuple<EventCondition, Character*, std::string_view, TargetCondition>;

struct EventData
{
    EventCondition type;                                     // The event enum
    std::string name = "";                                   // The name of the action cast
    TargetCondition statusCondition = TargetCondition::NONE; // The status boolean applied
    TraitCondition traitCondition = TraitCondition::NONE;    // The trait boolean applied
    int amount = 0;                                          // For damage, heal, or gain/lose X
    Character* source = nullptr;                             // Viewing pointer to the source entity
    Character* target = nullptr;                             // Viewing pointer to the target entity
};

struct EffectMessage
{
    EffectType type;
    std::string_view name;
    int amount = 0;
    BitsetWrapper<GameCondition> conditions;
    Character* source = nullptr;
    Character* target = nullptr;

    EffectMessage() = default;

    EffectMessage(EffectType tp, const std::string& n, Character& s, Character& t)
        : type(tp), name(n), source(&s), target(&t)
    {
    }
    EffectMessage(EffectType tp, const std::string& n, int number, Character& s, Character& t)
        : type(tp), name(n), amount(number), source(&s), target(&t)
    {
    }
};

// The BattleContext class will carry important information from another component to another
class BattleContext
{
  public:
    ActionType actionType;
    // Name of the action
    std::string name;
    // The character who executes the action
    Character& source;
    TargetScope scope;
    // Gathers all available targets from game class
    std::vector<std::unique_ptr<Character>>& enemies;
    std::vector<std::unique_ptr<Character>>& allies;
    std::vector<Character*> currentTargets;
    std::vector<int> damageDealt; // Will be removed, legacy member

    // Detects effect failure
    std::vector<Return_Flags> statusState;

    // For status effects
    int statusStrength;

    // Carries the generic effect message to the generic message component
    // Outputs "Target x applied y to target z"
    std::vector<EffectMessage> genericMessage;

    BattleContext(Character& c, Game& game);
};

class EventObserver
{
  private:
    std::queue<EventData> events;
    std::mutex mutex;
    std::condition_variable cv;
    bool gameQuit = false;

    // Synchronization for main thread waiting
    std::mutex processMutex;
    std::condition_variable processCv;
    bool eventProcessed = false;

    // Status effect helper methods
    bool isStatusRelevantEvent(EventCondition event);
    void checkAllStatusEffects(Game& game, EventCondition triggerEvent);
    void processCharacterStatuses(Game& game, Character* character, EventCondition triggerEvent);

  public:
    EventObserver() = default;
    void trigger(Game& game);
    void setQuit();
    void enqueue(EventData& event);
    void waitForEventProcessing();
};

#endif