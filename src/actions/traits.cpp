#include "actions/traits.h"
#include "core/effects.h"
#include "core/observer.h"
#include "ui/technical.h"

Trait::Trait(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

Return_Flags Trait::exhibit(Game& game, Character& player)
{
    BattleContext context(player, game);
    context.name = name;
    context.actionType = ActionType::TRAIT;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
              { return a->executionPriority < b->executionPriority; });

    // Trigger On_Exhibit Ability
    EventData event(EventCondition::ON_EXHIBIT, name, &player);
    Interface::eventBattleContext.enqueue(event);

    // Trigger On_Target_Exhibit Ability
    EventData target_event(EventCondition::ON_TARGET_EXHIBIT, name);
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
            message.conditions.set(GameCondition::FAILURE_TRAIT_ACTIVATION);
            context.genericMessage.push_back(message);
            std::cerr << "In a swirl of madness, you actually did nothing: ";
            break;
        }
    }
    return Return_Flags::SUCCESS;
}

void Trait::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}