#include "items.h"
#include "effects.h"
#include "observer.h"

Item::Item(const std::string& name_, const std::string& description_) : Action(name_, description_)
{
}

Return_Flags Item::use(Game& game, Character& player)
{
    BattleContext context(player, game);
    context.name = name;
    context.actionType = ActionType::ITEM;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
              { return a->executionPriority < b->executionPriority; });

    // Trigger On_Use Ability
    EventData event(EventCondition::ON_USE, name, &player);
    Interface::eventBattleContext.enqueue(event);

    // Trigger On_Target_Use Ability
    EventData target_event(EventCondition::ON_TARGET_USE, name);
    Interface::eventBattleContext.enqueue(target_event);

    Interface::eventBattleContext.waitForEventProcessing();

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
        }
        else
        {
            EffectMessage message = EffectMessage();
            message.conditions.set(GameCondition::FAILURE_ITEM_ACTIVATION);
            context.genericMessage.push_back(message);
            std::cerr << "Despite all the reasons you're here, your item consumed itself: ";
            break;
        }
    }
    return Return_Flags::SUCCESS;
}

void Item::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}