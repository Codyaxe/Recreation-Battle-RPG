

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
    EventCondition type;             // The event enum
    std::string name = "";           // The name of the action cast
    TargetCondition statusCondition; // The status boolean applied
    TraitCondition traitCondition;   // The trait boolean applied
    int amount = 0;                  // For damage, heal, or gain/lose X
    Character* source = nullptr;     // Pointer to the source entity (optional)
    Character* target = nullptr;     // Pointer to the target entity (optional)
};

// Will split this into multiple battle context instead of this one big dump, this will be changed
// into Events. I.E What is the type of the Event?
class BattleContext
{
  public:
    ActionType actionType;
    EffectType effectType;
    // Name of the action
    std::string name;
    // The character who executes the action
    Character& source;
    std::vector<std::unique_ptr<Character>>& enemies;
    std::vector<std::unique_ptr<Character>>& allies;
    std::vector<Character*> currentTargets;
    std::vector<Return_Flags> statusState;

    std::vector<int> damageDealt;

    TargetScope scope;
    ConditionContainer states;

    // Detects effect failure
    std::string failureReason;

    int statusStrength;

    // Carries the generic effect message to the generic message component
    std::vector<std::string> genericMessage;

    BattleContext(Character& c, Game& game);
};

class EventObserver
{
  private:
    std::queue<ObservedData> events;
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
    void enqueue(const EventCondition& event, Character* c = nullptr, std::string_view str = {},
                 const TargetCondition& condition = TargetCondition::NONE);
    void waitForEventProcessing();
};

#endif