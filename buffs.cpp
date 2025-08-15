#include "buffs.h"
#include "effects.h"
#include "observer.h"

Buff::Buff(const std::string& name_, const std::string& description_) : Action(name_, description_)
{
}

Return_Flags Buff::buff(Game& game, Character& player)
{
    Observer context(player, game);
    context.name = name;
    context.actionType = ActionType::BUFF;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
              { return a->executionPriority < b->executionPriority; });

    // Execute components in order
    for (auto& component : components)
    {
        // Checks activation condition first before executing
        if (component->canExecute(context))
        {
            // If targeting was exited return back to the menu, false indicates user exited
            if (component->execute(context) == Return_Flags::EXIT)
            {
                return Return_Flags::EXIT;
            }

            if (context.states.game.has(GameCondition::FAILED) && !component->isOptional)
            {
                std::cout << "Unfortunately, it was a placebo all along: " << context.failureReason
                          << std::endl;
                break;
            }
        }
    }
    return Return_Flags::SUCCESS;
}

void Buff::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}