#include "attacks.h"
#include "effects.h"
#include "observer.h"

Attack::Attack(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

Return_Flags Attack::initiate(Game& game, Character& player)
{
    BattleContext context(player, game);
    context.name = name;
    context.actionType = ActionType::ATTACK;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
              { return a->executionPriority < b->executionPriority; });

    // Trigger On_Initiate Ability
    EventData event(EventCondition::ON_INITIATE, name, &player);
    Interface::eventBattleContext.enqueue(event);

    // Trigger On_Target_Initiate Ability
    EventData target_event(EventCondition::ON_TARGET_INITIATE, name);
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
            message.conditions.set(GameCondition::FAILURE_ATTACK_ACTIVATION);
            context.genericMessage.push_back(message);
            std::cout << "As you make your way to your enemy, you forged a dream towards "
                      << "nonexistence.";
            break;
        }
    }
    return Return_Flags::SUCCESS;
}

void Attack::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}