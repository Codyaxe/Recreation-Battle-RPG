

// Handles UI, STATS, AND PERHAPS SOUND
// GameEventObserver interface
// GameEventSubject base class
// event structures (SpellCastEvent, etc.)
// concrete observers (UIObserver, StatsObserver, etc.)
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

class Observer
{
  public:
    ActionType actionType;
    EffectType effectType;
    std::string name;
    Character& caster;
    std::vector<std::unique_ptr<Character>>& enemies;
    std::vector<std::unique_ptr<Character>>& allies;
    std::vector<Character*> currentTargets;
    std::vector<int> damageDealt;
    std::vector<Return_Flags> statusState;
    TargetScope scope;
    ConditionContainer states;
    std::string failureReason;
    int targetsDefeated = 0;
    // For Status Effect
    int statusStrength;

    Observer(Character& c, Game& game);
};

class GlobalEventObserver
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
    GlobalEventObserver() = default;
    void trigger(Game& game);
    void setQuit();
    void enqueue(const EventCondition& event, Character* c = nullptr, std::string_view str = {},
                 const TargetCondition& condition = TargetCondition::NONE);
    void waitForEventProcessing(); // New method for main thread to wait
};

#endif