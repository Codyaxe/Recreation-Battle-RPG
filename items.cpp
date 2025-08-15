#include "items.h"
#include "effects.h"
#include "observer.h"

Item::Item(const std::string& name_, const std::string& description_) : Action(name_, description_)
{
}

Return_Flags Item::use(Game& game, Character& player)
{
    Observer context(player, game);
    context.name = name;
    context.actionType = ActionType::ITEM;

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
                std::cout << "Despite all the reasons you're here, your item consumed itself: "
                          << context.failureReason << std::endl;
                break;
            }
        }
    }
    return Return_Flags::SUCCESS;
}

void Item::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}