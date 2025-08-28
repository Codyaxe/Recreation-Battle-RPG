#include "observer.h"
#include "status.h"
#include "character.h"
#include "technical.h"
#include <algorithm>
#include <unordered_set>
#include <thread>
#include <chrono>

BattleContext::BattleContext(Character& c, Game& game)
    : source(c), enemies(game.enemies), allies(game.allies)
{
}

void EventObserver::enqueue(EventData& event)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        events.push(event);
    }

    // Reset the processed flag before notifying
    {
        std::lock_guard<std::mutex> processLock(processMutex);
        eventProcessed = false;
    }

    cv.notify_one(); // Notify waiting thread
}

void ::EventObserver::setQuit()
{
    std::lock_guard<std::mutex> lock(mutex);
    gameQuit = true;
    cv.notify_one();
}

void EventObserver::waitForEventProcessing()
{
    std::unique_lock<std::mutex> lock(processMutex);
    processCv.wait(lock, [this] { return eventProcessed; });
}

bool EventObserver::isStatusRelevantEvent(EventCondition event)
{
    static const std::unordered_set<EventCondition> statusEvents = {
        EventCondition::ON_STATUS_TICK,
        EventCondition::ON_START_TURN,
        EventCondition::ON_END_TURN,
        EventCondition::ON_DAMAGE_TAKEN,
        EventCondition::ON_DEALING_DAMAGE,
        EventCondition::ON_HEAL,
        EventCondition::ON_DEATH,
        EventCondition::ON_CRIT,
        EventCondition::ON_BLOCK,
        EventCondition::ON_PARRY,
        EventCondition::ON_DODGE,
        EventCondition::ON_MISS,
        EventCondition::ON_KILL};
    return statusEvents.find(event) != statusEvents.end();
}

void EventObserver::checkAllStatusEffects(Game& game, EventCondition triggerEvent)
{
    if (!isStatusRelevantEvent(triggerEvent))
    {
        return;
    }

    for (auto& ally : game.allies)
    {
        if (!ally->statuses.empty())
        {
            processCharacterStatuses(game, ally.get(), triggerEvent);
        }
    }

    for (auto& enemy : game.enemies)
    {
        if (!enemy->statuses.empty())
        {
            processCharacterStatuses(game, enemy.get(), triggerEvent);
        }
    }
}

void EventObserver::processCharacterStatuses(Game& game, Character* character,
                                             EventCondition triggerEvent)
{
    std::cout << "Processing status effects for character: " << character->name
              << " on event: " << static_cast<int>(triggerEvent) << std::endl;
    Sleep(1000);

    auto it = character->statuses.begin();
    while (it != character->statuses.end())
    {
        const auto& status = *it;
        const auto& activationConditions = status->activationConditions;

        // Check if this status should trigger on the current event
        bool shouldTrigger = std::any_of(activationConditions.begin(), activationConditions.end(),
                                         [triggerEvent](const auto& condition)
                                         { return condition == triggerEvent; });

        if (triggerEvent == EventCondition::ON_END_TURN)
        {
            status->duration -= 1;
            EventData statusTickEvent(EventCondition::ON_STATUS_TICK, character);
            enqueue(statusTickEvent);
        }

        if (shouldTrigger)
        {
            std::cout << "Triggering status effect: " << status->name
                      << " (Duration: " << status->duration << ")" << std::endl;
            Sleep(1000);

            status->trigger(game, character);

            // Remove expired status effects
            if (status->duration <= 0)
            {
                std::cout << "Status effect expired and removed: " << status->name << std::endl;
                Sleep(1000);

                status->expire(game, character);
                EventData statusExpiredEvent(EventCondition::ON_STATUS_EXPIRE, character);
                enqueue(statusExpiredEvent);

                it = character->statuses.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void EventObserver::trigger(Game& game)
{
    // Event maps for different handler signatures
    bool success = true;

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
        statusConditionEventMap = {
            {EventCondition::ON_GAIN_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onGainX(g, cond); }},
            {EventCondition::ON_LOSE_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onLoseX(g, cond); }},
        };

    static const std::unordered_map<EventCondition,
                                    std::function<void(Character*, Game&, TraitCondition)>>
        traitConditionEventMap = {
            {EventCondition::ON_GAIN_X,
             [](Character* c, Game& g, TraitCondition cond) { c->onGainX(g, cond); }},
            {EventCondition::ON_LOSE_X,
             [](Character* c, Game& g, TraitCondition cond) { c->onLoseX(g, cond); }},
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

    static const std::unordered_map<EventCondition, std::function<void(Character*, Game&)>>
        genericEventMap = {
            {EventCondition::ON_START_TURN, [](Character* c, Game& g) { c->onTurnStart(g); }},
            {EventCondition::ON_END_TURN, [](Character* c, Game& g) { c->onTurnEnd(g); }}};

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
            EventData& data = events.front();
            lock.unlock(); // Release lock early

            if (data.source)
            {
                // Handle events with string parameter
                if (!data.name.empty())
                {
                    auto it = stringEventMap.find(data.type);
                    if (it != stringEventMap.end())
                    {
                        if (data.source->onEventsAbilities.has(data.type))
                        {
                            it->second(data.source, game, data.name);
                        }
                        else
                        {
                            success = false;
                        }
                        checkAllStatusEffects(game, data.type);
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND STR: " << static_cast<int>(data.type)
                                  << '\n';
                    }
                }
                // Handle events with status condition parameter
                else if (data.statusCondition != TargetCondition::NONE)
                {
                    auto it = statusConditionEventMap.find(data.type);
                    if (it != statusConditionEventMap.end())
                    {
                        if (data.source->onEventsAbilities.has(data.type))
                        {
                            it->second(data.source, game, data.statusCondition);
                        }
                        else
                        {
                            success = false;
                        }
                        checkAllStatusEffects(game, data.type);
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND C Target: "
                                  << static_cast<int>(data.type) << '\n';
                    }
                }
                // Handle events with trait condition parameter
                else if (data.traitCondition != TraitCondition::NONE)
                {
                    auto it = traitConditionEventMap.find(data.type);
                    if (it != traitConditionEventMap.end())
                    {
                        if (data.source->onEventsAbilities.has(data.type))
                        {
                            it->second(data.source, game, data.traitCondition);
                        }
                        else
                        {
                            success = false;
                        }

                        checkAllStatusEffects(game, data.type);
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND C: " << static_cast<int>(data.type)
                                  << '\n';
                    }
                }
                // Handle simple events character parameters only
                else
                {
                    auto it = simpleEventMap.find(data.type);
                    if (it != simpleEventMap.end())
                    {
                        if (data.source->onEventsAbilities.has(data.type))
                        {
                            it->second(data.source, game);
                        }
                        else
                        {
                            success = false;
                        }
                        checkAllStatusEffects(game, data.type);
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND S: " << static_cast<int>(data.type)
                                  << '\n';
                    }
                }
            }
            else
            {
                // Handle target events
                if (!data.name.empty())
                {
                    auto it = targetEventMap.find(data.type);
                    if (it != targetEventMap.end())
                    {
                        auto callEvent = [&](Character* target)
                        {
                            if (target->onEventsAbilities.has(data.type))
                            {
                                it->second(data.source, game, data.name);
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
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND TAR: " << static_cast<int>(data.type)
                                  << '\n';
                    }
                    checkAllStatusEffects(game, data.type);
                }
                else
                {
                    auto it = genericEventMap.find(data.type);
                    if (it != genericEventMap.end())
                    {
                        auto callEvent = [&](Character* target)
                        {
                            if (target->onEventsAbilities.has(data.type))
                            {
                                it->second(target, game);
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
                    }
                    else
                    {
                        std::cerr << "ERROR! EVENT NOT FOUND G: " << static_cast<int>(data.type)
                                  << '\n';
                    }
                    checkAllStatusEffects(game, data.type);
                }
            }

            // Allow user to skip status effect animations with condition variable
            std::mutex skipMutex;
            std::condition_variable skipCv;
            bool skipPressed = false;
            bool shouldExit = false;
            events.pop();

            if (success)
            {
                std::cout << "Press Enter to continue." << '\n';
            }

            // Skip option for status effect
            std::thread listener([&]()
                                 { processSkip(skipMutex, skipCv, skipPressed, shouldExit); });
            if (success)
            {
                {
                    // Signals exit after 5 seconds if the enter key is not pressed
                    std::unique_lock<std::mutex> lock(skipMutex);

                    skipCv.wait_for(lock, std::chrono::seconds(5),
                                    [&skipPressed] { return skipPressed; });
                    shouldExit = true;
                }
            }
            else
            {
                {
                    std::unique_lock<std::mutex> lock(skipMutex);
                    shouldExit = true;
                    success = true;
                }
            }

            listener.join();

            if (events.empty())
            {
                {
                    std::lock_guard<std::mutex> processLock(processMutex);
                    eventProcessed = true;
                }
                processCv.notify_one();
            }
        }
    }
}
