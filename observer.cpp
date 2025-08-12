#include "observer.h"
#include "character.h"
#include "technical.h"

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
}

void EventObserver::trigger(Game& game, Event_Type event, Character& c)
{
    while (true)
    {
        events.push(event);
        if (!events.empty())
        {
            for (auto& ally : game.allies)
            {
                // Trigger Their On-Event Abilities
            }
            for (auto& ally : game.allies)
            {
                // Trigger Their On-Event Abilities
            }
        }
    }
}