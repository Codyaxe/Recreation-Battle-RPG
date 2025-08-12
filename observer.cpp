#include "observer.h"
#include "character.h"
#include "technical.h"

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
}

void GlobalEventObserver::enqueue(const EventConditions& event, Character* c, std::string_view str,
                                  const TargetCondition& condition)
{
    std::lock_guard<std::mutex> lock(mutex);
    ObservedData eventToTrigger = {event, c, str, condition};
    events.push(eventToTrigger);
}

void GlobalEventObserver::trigger(Game& game)
{
    // Event maps for different handler signatures
    static const std::unordered_map<EventConditions,
                                    std::function<void(Character*, Game&, std::string_view)>>
        stringEventMap = {
            {EventConditions::ON_INITIATE,
             [](Character* c, Game& g, std::string_view s) { c->onInitiate(g, s); }},
            {EventConditions::ON_CAST,
             [](Character* c, Game& g, std::string_view s) { c->onCast(g, s); }},
            {EventConditions::ON_AFFLICT,
             [](Character* c, Game& g, std::string_view s) { c->onAfflict(g, s); }},
            {EventConditions::ON_BUFF,
             [](Character* c, Game& g, std::string_view s) { c->onBuff(g, s); }},
            {EventConditions::ON_EXHIBIT,
             [](Character* c, Game& g, std::string_view s) { c->onExhibit(g, s); }},
            {EventConditions::ON_CALL,
             [](Character* c, Game& g, std::string_view s) { c->onCall(g, s); }},
            {EventConditions::ON_USE,
             [](Character* c, Game& g, std::string_view s) { c->onUse(g, s); }},
            {EventConditions::ON_CHANGE,
             [](Character* c, Game& g, std::string_view s) { c->onChange(g, s); }},
            {EventConditions::ON_PLAY,
             [](Character* c, Game& g, std::string_view s) { c->onPlay(g, s); }},
        };

    static const std::unordered_map<EventConditions,
                                    std::function<void(Character*, Game&, TargetCondition)>>
        conditionEventMap = {
            {EventConditions::ON_GAIN_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onGainX(g, cond); }},
            {EventConditions::ON_LOSE_X,
             [](Character* c, Game& g, TargetCondition cond) { c->onLoseX(g, cond); }},
        };

    static const std::unordered_map<EventConditions, std::function<void(Character*, Game&)>>
        simpleEventMap = {
            {EventConditions::ON_DEATH, [](Character* c, Game& g) { c->onDeath(g); }},
            {EventConditions::ON_SUMMON, [](Character* c, Game& g) { c->onSummon(g); }},
            {EventConditions::ON_DAMAGE_TAKEN, [](Character* c, Game& g) { c->onTakingDamage(g); }},
            {EventConditions::ON_DEALING_DAMAGE,
             [](Character* c, Game& g) { c->onDealingDamage(g); }},
            {EventConditions::ON_HEAL, [](Character* c, Game& g) { c->onHeal(g); }},
            {EventConditions::ON_CRIT, [](Character* c, Game& g) { c->onCrit(g); }},
            {EventConditions::ON_BLOCK, [](Character* c, Game& g) { c->onBlock(g); }},
            {EventConditions::ON_PARRY, [](Character* c, Game& g) { c->onParry(g); }},
            {EventConditions::ON_DODGE, [](Character* c, Game& g) { c->onDodge(g); }},
            {EventConditions::ON_MISS, [](Character* c, Game& g) { c->onMiss(g); }},
            {EventConditions::ON_KILL, [](Character* c, Game& g) { c->onKill(g); }},
            {EventConditions::ON_FATAL_DAMAGE,
             [](Character* c, Game& g) { c->onTakingFatalDamage(g); }},
        };

    static const std::unordered_map<EventConditions,
                                    std::function<void(Character*, Game&, std::string_view)>>
        targetEventMap = {
            {EventConditions::ON_TARGET_INITIATE,
             [](Character* c, Game& g, std::string_view s) { c->onInitiate(g, s); }},
            {EventConditions::ON_TARGET_CAST,
             [](Character* c, Game& g, std::string_view s) { c->onTargetCast(g, s); }},
            {EventConditions::ON_TARGET_AFFLICT,
             [](Character* c, Game& g, std::string_view s) { c->onTargetAfflict(g, s); }},
            {EventConditions::ON_TARGET_BUFF,
             [](Character* c, Game& g, std::string_view s) { c->onTargetBuff(g, s); }},
            {EventConditions::ON_TARGET_EXHIBIT,
             [](Character* c, Game& g, std::string_view s) { c->onTargetExhibit(g, s); }},
            {EventConditions::ON_TARGET_CALL,
             [](Character* c, Game& g, std::string_view s) { c->onTargetCall(g, s); }},
            {EventConditions::ON_TARGET_USE,
             [](Character* c, Game& g, std::string_view s) { c->onTargetUse(g, s); }},
            {EventConditions::ON_TARGET_CHANGE,
             [](Character* c, Game& g, std::string_view s) { c->onTargetChange(g, s); }},
            {EventConditions::ON_TARGET_PLAY,
             [](Character* c, Game& g, std::string_view s) { c->onTargetPlay(g, s); }},
        };

    while (true)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!events.empty())
        {
            ObservedData data = events.front();
            events.pop();
            lock.~lock_guard();

            EventConditions event = std::get<0>(data);
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
                        callEvent(ally);
                    }
                    for (auto& enemy : game.enemies)
                    {
                        callEvent(enemy);
                    }
                    continue;
                }
            }

            std::cout << "ERROR! EVENT NOT FOUND: " << static_cast<int>(event) << '\n';
            Sleep(1000);
        }
    }
}
