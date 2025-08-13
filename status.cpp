#include "status.h"
#include "effects.h"
#include "observer.h"

Status::Status(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

bool Status::trigger(Game& game, Character* player)
{
    Observer context(*player, game); // Grabs strength
    context.name = name;
    context.type = ActionType::STATUS;
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
            if (!component->execute(context))
            {
                return false;
            }

            if (context.states.game.has(GameCondition::FAILED) && !component->isOptional)
            {
                std::cout << "The status effect failed!! WHY??? " << context.failureReason
                          << std::endl;
                break;
            }
        }
    }
    return true;
}

void Status::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}

namespace PoisonText
{
static const char* const TEXTS[] = {"You took poison damage!"};
static constexpr size_t TEXT_COUNT = sizeof(TEXTS) / sizeof(TEXTS[0]);
} // namespace PoisonText

// Template Status
Poison::Poison() : Status("Poison Status Effect", "A debilitating tick damage")
{

    // Do Not Implement Targeting For Status Effect.
    //  Effect component
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
        ui->primaryTexts.push_back(MessageComponent::PrimaryText(PoisonText::TEXTS[i], true, 50));
    }
    ui->executionPriority = 2;
    addComponent(std::move(ui));
}