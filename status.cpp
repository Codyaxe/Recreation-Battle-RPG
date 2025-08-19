#include "status.h"
#include "effects.h"
#include "character.h"
#include "observer.h"

Status::Status(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

Return_Flags Status::trigger(Game& game, Character* player)
{
    BattleContext context(*player, game); // Grabs strength
    context.name = name;
    context.actionType = ActionType::STATUS;
    context.statusStrength = strength;
    // Status Effects targets the player who has it.
    context.currentTargets.push_back(player);

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
        }
        else
        {
            EffectMessage message = EffectMessage();
            message.conditions.set(GameCondition::FAILURE_STATUS_ACTIVATION);
            context.genericMessage.push_back(message);
            std::cerr << "The status effect failed!! WHY??? ";
            break;
        }
    }
    return Return_Flags::SUCCESS;
}

Return_Flags Status::expire(Game& game, Character* player)
{
    player->targetConditions.clear(condition);
    return Return_Flags::SUCCESS;
}

void Status::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}

namespace PoisonText
{
static const char* const TEXTS[] = {"Took poison damage!"};
static constexpr size_t TEXT_COUNT = sizeof(TEXTS) / sizeof(TEXTS[0]);
} // namespace PoisonText

// Template Status
Poison::Poison() : Status("Poison Status Effect", "A debilitating tick damage")
{

    // Do not add the targeting component for status effect
    // Default Arguments
    condition = TargetCondition::POISON;
    duration = 3;
    strength = 1;

    activationConditions.push_back(EventCondition::ON_END_TURN);
    auto effect = std::make_unique<EffectComponent>();
    auto primaryFX = EffectComponent::PrimaryEffect(EffectType::DAMAGE, "Poison",
                                                    DynamicValue(20, DamageBasis::STATUS_STRENGTH));
    effect->primaryEffects.push_back(std::move(primaryFX));
    effect->executionPriority = 1;
    addComponent(std::move(effect));

    // UI component
    auto ui = std::make_unique<MessageComponent>();
    ui->primaryTexts.reserve(PoisonText::TEXT_COUNT); // Pre-allocate for efficiency
    for (size_t i = 0; i < PoisonText::TEXT_COUNT; ++i)
    {
        ui->primaryTexts.push_back(
            MessageComponent::PrimaryText(PoisonText::TEXTS[i], false, true, 50));
    }
    ui->executionPriority = 2;
    addComponent(std::move(ui));
}

Poison::Poison(int dur, int str) : Poison()
{
    duration = dur;
    strength = str;
}