#include "observer.h"
#include "character.h"
#include "technical.h"
#include <unordered_set>

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
}

void GlobalEventObserver::enqueue(const EventCondition& event, Character* c, std::string_view str,
                                  const TargetCondition& condition)
{
    std::lock_guard<std::mutex> lock(mutex);
    ObservedData eventToTrigger = {event, c, str, condition};
    events.push(eventToTrigger);
    cv.notify_one(); // Notify waiting thread
}

void ::GlobalEventObserver::setQuit()
{
    std::lock_guard<std::mutex> lock(mutex);
    gameQuit = true;
    cv.notify_one();
}

// bool GlobalEventObserver::isStatusRelevantEvent(EventCondition event)
// {
//     static const std::unordered_set<EventCondition> statusEvents = {
//         EventCondition::ON_STATUS_TICK,
//         EventCondition::ON_START_TURN,
//         EventCondition::ON_END_TURN,
//         EventCondition::ON_DAMAGE_TAKEN,
//         EventCondition::ON_DEALING_DAMAGE,
//         EventCondition::ON_HEAL,
//         EventCondition::ON_DEATH,
//         EventCondition::ON_CRIT,
//         EventCondition::ON_BLOCK,
//         EventCondition::ON_PARRY,
//         EventCondition::ON_DODGE,
//         EventCondition::ON_MISS,
//         EventCondition::ON_KILL};
//     return statusEvents.find(event) != statusEvents.end();
// }

// void GlobalEventObserver::checkAllStatusEffects(Game& game, EventCondition triggerEvent)
// {
//     if (!isStatusRelevantEvent(triggerEvent))
//         return;

//     for (auto& ally : game.allies)
//     {
//         if (!ally->statuses.empty())
//         {
//             processCharacterStatuses(game, ally.get(), triggerEvent);
//         }
//     }

//     for (auto& enemy : game.enemies)
//     {
//         if (!enemy->statuses.empty())
//         {
//             processCharacterStatuses(game, enemy.get(), triggerEvent);
//         }
//     }
// }

// void GlobalEventObserver::processCharacterStatuses(Game& game, Character* character,
//                                                    EventCondition triggerEvent)
// {
//     auto it = character->statuses.begin();
//     while (it != character->statuses.end())
//     {
//         if (it->shouldTriggerOn(triggerEvent))
//         {
//             bool statusContinues = it->trigger(game, character);
//             if (!statusContinues || it->isExpired())
//             {
//                 it = character->statuses.erase(it);
//                 continue;
//             }
//         }
//         ++it;
//     }
// }

void GlobalEventObserver::trigger(Game& game)
{
    // Event maps for different handler signatures
    static const std::unordered_map<EventCondition,
                                    std::function<void(Character*, Game&, std::string_view)>>
        stringEventMap = {
            {EventCondition::ON_INITIATE,
             [](Character* c, Game& g, std::string_view s) { c->onInitiate(g, s); }},
            {EventCondition::ON_CAST,
             [](Character* c, Game& g, std::string_view s) { c->onCast(g, s); }},
            {EventCondition::ON_AFFLICT,
             [](Character* c, Game& g, std::string_view s) { c->onAfflict(g, s); }},
            {EventCondition::ON_BUFF,
             [](Character* c, Game& g, std::string_view s) { c->onBuff(g, s); }},
            {EventCondition::ON_EXHIBIT,
             [](Character* c, Game& g, std::string_view s) { c->onExhibit(g, s); }},
            {EventCondition::ON_CALL,
             [](Character* c, Game& g, std::string_view s) { c->onCall(g, s); }},
            {EventCondition::ON_USE,
             [](Character* c, Game& g, std::string_view s) { c->onUse(g, s); }},
            {EventCondition::ON_CHANGE,
             [](Character* c, Game& g, std::string_view s) { c->onChange(g, s); }},
            {EventCondition::ON_PLAY,
             [](Character* c, Game& g, std::string_view s) { c->onPlay(g, s); }},
        };

    static const std::unordered_map<EventCondition,
                                    std::function<void(Character*, Game&, TargetCondition)>>
        conditionEventMap = {
            {EventCondition::ON_GAIN_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onGainX(g, cond); }},
            {EventCondition::ON_LOSE_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onLoseX(g, cond); }},
        };

    static const std::unordered_map<EventCondition, std::function<void(Character*, Game&)>>
        simpleEventMap = {
            {EventCondition::ON_DEATH, [](Character* c, Game& g) { c->onDeath(g); }},
            {EventCondition::ON_SUMMON, [](Character* c, Game& g) { c->onSummon(g); }},
            {EventCondition::ON_DAMAGE_TAKEN, [](Character* c, Game& g) { c->onTakingDamage(g); }},
            {EventCondition::ON_DEALING_DAMAGE,
             [](Character* c, Game& g) { c->onDealingDamage(g); }},
            {EventCondition::ON_HEAL, [](Character* c, Game& g) { c->onHeal(g); }},
            {EventCondition::ON_CRIT, [](Character* c, Game& g) { c->onCrit(g); }},
            {EventCondition::ON_BLOCK, [](Character* c, Game& g) { c->onBlock(g); }},
            {EventCondition::ON_PARRY, [](Character* c, Game& g) { c->onParry(g); }},
            {EventCondition::ON_DODGE, [](Character* c, Game& g) { c->onDodge(g); }},
            {EventCondition::ON_MISS, [](Character* c, Game& g) { c->onMiss(g); }},
            {EventCondition::ON_KILL, [](Character* c, Game& g) { c->onKill(g); }},
            {EventCondition::ON_FATAL_DAMAGE,
             [](Character* c, Game& g) { c->onTakingFatalDamage(g); }},
        };

    static const std::unordered_map<EventCondition,
                                    std::function<void(Character*, Game&, std::string_view)>>
        targetEventMap = {
            {EventCondition::ON_TARGET_INITIATE,
             [](Character* c, Game& g, std::string_view s) { c->onInitiate(g, s); }},
            {EventCondition::ON_TARGET_CAST,
             [](Character* c, Game& g, std::string_view s) { c->onTargetCast(g, s); }},
            {EventCondition::ON_TARGET_AFFLICT,
             [](Character* c, Game& g, std::string_view s) { c->onTargetAfflict(g, s); }},
            {EventCondition::ON_TARGET_BUFF,
             [](Character* c, Game& g, std::string_view s) { c->onTargetBuff(g, s); }},
            {EventCondition::ON_TARGET_EXHIBIT,
             [](Character* c, Game& g, std::string_view s) { c->onTargetExhibit(g, s); }},
            {EventCondition::ON_TARGET_CALL,
             [](Character* c, Game& g, std::string_view s) { c->onTargetCall(g, s); }},
            {EventCondition::ON_TARGET_USE,
             [](Character* c, Game& g, std::string_view s) { c->onTargetUse(g, s); }},
            {EventCondition::ON_TARGET_CHANGE,
             [](Character* c, Game& g, std::string_view s) { c->onTargetChange(g, s); }},
            {EventCondition::ON_TARGET_PLAY,
             [](Character* c, Game& g, std::string_view s) { c->onTargetPlay(g, s); }},
        };

    while (true)
    {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait for events or quit signal
        cv.wait(lock, [this] { return !events.empty() || gameQuit; });

        if (gameQuit && events.empty())
        {
            std::cerr << "Game Successfully Quited!" << '\n';
            Sleep(1000);
            break;
        }

        if (!events.empty())
        {
            ObservedData data = events.front();
            events.pop();
            lock.unlock(); // Release lock early

            EventCondition event = std::get<0>(data);
            Character* target = std::get<1>(data);
            std::string_view str = std::get<2>(data);
            TargetCondition condition = std::get<3>(data);

            if (target)
            {
                // Handle events with string parameter
                if (!str.empty())
                {
                    auto it = stringEventMap.find(event);
                    if (it != stringEventMap.end())
                    {
                        it->second(target, game, str);
                        // checkAllStatusEffects(game, event);
                        continue;
                    }
                }
                // Handle events with condition parameter
                else if (condition != TargetCondition::NONE)
                {
                    auto it = conditionEventMap.find(event);
                    if (it != conditionEventMap.end())
                    {
                        it->second(target, game, condition);
                        // checkAllStatusEffects(game, event);
                        continue;
                    }
                }
                // Handle simple events (no extra parameters)
                else
                {
                    auto it = simpleEventMap.find(event);
                    if (it != simpleEventMap.end())
                    {
                        it->second(target, game);
                        // checkAllStatusEffects(game, event);
                        continue;
                    }
                }
            }
            else
            {
                // Handle target events
                auto it = targetEventMap.find(event);
                if (it != targetEventMap.end())
                {
                    auto callEvent = [&](Character* character)
                    {
                        if (character->onEventsAbilities.has(event))
                        {
                            it->second(character, game, str);
                        }
                    };

                    for (auto& ally : game.allies)
                    {
                        callEvent(ally.get());
                    }
                    for (auto& enemy : game.enemies)
                    {
                        callEvent(enemy.get());
                    }
                    // checkAllStatusEffects(game, event);
                    continue;
                }
            }

            std::cout << "ERROR! EVENT NOT FOUND: " << static_cast<int>(event) << '\n';
            Sleep(100); // Reduced sleep time for errors
        }
    }
}
