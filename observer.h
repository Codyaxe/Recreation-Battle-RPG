

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
#include <tuple>
#include <functional>
#include <string_view>

using ObservedData = std::tuple<EventConditions, Character*, std::string_view, TargetCondition>;

class Observer
{
  public:
    ActionType type;
    std::string name;
    Character& caster;
    std::vector<Character*>& enemies;
    std::vector<Character*>& allies;
    std::vector<Character*> currentTargets;
    std::vector<int> damageDealt;
    TargetScope scope;
    ConditionContainer states;
    std::string failureReason;
    int targetsDefeated = 0;

    Observer(Character& c, Game& game);
};

class GlobalEventObserver
{
  private:
    std::queue<ObservedData> events;
    std::mutex mutex;

  public:
    GlobalEventObserver() = default;
    void trigger(Game& game);
    void enqueue(const EventConditions& event, Character* c = nullptr, std::string_view str = {},
                 const TargetCondition& condition = TargetCondition::NONE);
};

#endif