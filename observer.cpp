#include "observer.h"
#include "technical.h"

Observer::Observer(Character& c, Game& game)
    : caster(c), enemies(game.enemies), allies(game.allies), states()
{
    states.game = game.gameConditions;
}
