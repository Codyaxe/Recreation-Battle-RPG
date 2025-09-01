#include "actions/spells.h"
#include "core/status.h"
#include "core/effects.h"
#include "core/observer.h"
#include "ui/technical.h"

Spell::Spell(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

void Spell::addComponent(std::unique_ptr<Component> component)
{
    Action::addComponent(std::move(component));
}

Return_Flags Spell::cast(Game& game, Character& player)
{
    BattleContext context(player, game);
    context.name = name;
    context.actionType = ActionType::SPELL;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b)
              { return a->executionPriority < b->executionPriority; });

    // Trigger On_Cast Ability
    EventData event(EventCondition::ON_CAST, name, &player);
    Interface::eventBattleContext.enqueue(event);

    // Trigger On_Target_Cast Ability
    EventData target_event(EventCondition::ON_TARGET_CAST, name);
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
            message.conditions.set(GameCondition::FAILURE_SPELL_ACTIVATION);
            context.genericMessage.push_back(message);
            std::cout << "As you chanted the spell, you suddenly forgot how to speak: ";
            break;
        }
    }
    return Return_Flags::SUCCESS;
}

/*For making string cost cheaper in case I would reuse this. Could use string_view but this one is
/ cheaper */
namespace FireballText
{
static const char* const TEXTS[] = {
    "Flames swirl and gather in your hands, forming a searing sphere of molten energy.",
    "The fireball crackles with power, casting flickering light across the battlefield.",
    "With a sharp thrust, you launch it toward your chosen foe.",
    "It streaks through the air, leaving a shimmering trail of heat and smoke in its wake.",
    "Upon impact, it bursts in a thunderous explosion, engulfing the target in a roaring inferno!"};
static constexpr size_t TEXT_COUNT = sizeof(TEXTS) / sizeof(TEXTS[0]);
} // namespace FireballText

// Template Spell
Fireball::Fireball() : Spell("Fireball", "A fiery blast against an enemy.")
{
    // Targeting component
    auto targeting = std::make_unique<TargetingComponent>();
    targeting->mode = TargetSelectionMode::MANUAL;
    targeting->scope = TargetScope::SINGLE;
    targeting->faction = TargetFaction::ENEMIES;
    targeting->executionPriority = 1;
    addComponent(std::move(targeting));

    // Effect component
    auto effect = std::make_unique<EffectComponent>();
    auto primaryFX = EffectComponent::PrimaryEffect(EffectType::DAMAGE, "Fire", DynamicValue(200));
    effect->primaryEffects.push_back(std::move(primaryFX));
    effect->executionPriority = 2;
    addComponent(std::move(effect));

    // UI component
    auto ui = std::make_unique<MessageComponent>();
    ui->primaryTexts.reserve(FireballText::TEXT_COUNT); // Pre-allocate for efficiency
    for (size_t i = 0; i < FireballText::TEXT_COUNT; ++i)
    {
        ui->primaryTexts.push_back(MessageComponent::PrimaryText(FireballText::TEXTS[i], true, 50));
    }
    ui->executionPriority = 3;
    addComponent(std::move(ui));
}

Poison_Gas::Poison_Gas() : Spell("Poison Gas", "A gas that poisons your target.")
{
    // Targeting component
    auto targeting = std::make_unique<TargetingComponent>();
    targeting->mode = TargetSelectionMode::MANUAL;
    targeting->scope = TargetScope::SINGLE;
    targeting->faction = TargetFaction::ENEMIES;
    targeting->executionPriority = 1;
    addComponent(std::move(targeting));

    // Effect component
    auto effect = std::make_unique<EffectComponent>();
    auto primaryFX = EffectComponent::PrimaryEffect(EffectType::DEBUFF, TargetCondition::POISON,
                                                    std::make_unique<Poison>(5, 2));
    effect->primaryEffects.push_back(std::move(primaryFX));
    effect->executionPriority = 2;
    addComponent(std::move(effect));

    // UI component
    auto ui = std::make_unique<MessageComponent>();
    ui->primaryTexts.push_back(
        MessageComponent::PrimaryText("Poisonous gas swirls around your enemy!", true, 50));
    ui->executionPriority = 3;
    addComponent(std::move(ui));
}
