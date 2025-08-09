#include "spells.h"
#include "character.h"
#include "components.h"
#include "spelleffects.h"
#include "technical.h"

// class Spell{

//     private:

//     std::string name_;
//     std::string description_;
//     EffectFunction effect_;

//

//     Spell(std::string& name, std::string& description) :
//     name_(name), description_(description), effect_(effect)
//     {}

//     virtual void effect(Character& target) = 0;

// };

Spell::Spell(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

void Spell::addComponent(std::unique_ptr<Components> component)
{
    components.push_back(std::move(component));
}

void Spell::cast(Game& game, Character& player)
{
    Observer context(player, game);

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Components>& a, const std::unique_ptr<Components>& b)
              { return a->executionPriority < b->executionPriority; });

    // Execute components in order
    for (auto& component : components)
    {
        if (component->canExecute(context))
        {
            component->execute(context);

            // Stop execution if spell failed and component is not optional
            if (context.states.game.has(GameCondition::SpellFailed) && !component->isOptional)
            {
                std::cout << "Spell failed: " << context.failureReason << std::endl;
                break;
            }
        }
    }
}

Fireball::Fireball() : Spell("Fireball", "A fiery blast against an enemy.") {}

void Fireball::cast(Game& game, Character& player)
{
    std::vector<int> targets = chooseTarget(game.enemies, game.allies, name, 1, ENEMIES, SPELL);
    applyDamage(game.enemies[targets[0]], 150, FIRE);
}

void Fireball::operator()(Game& game, Character& player) { cast(game, player); }
