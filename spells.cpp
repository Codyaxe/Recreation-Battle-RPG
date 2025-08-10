#include "spells.h"
#include "character.h"
#include "components.h"
#include "effects.h"
#include "observer.h"
#include "technical.h"

Spell::Spell(const std::string& name_, const std::string& description_)
    : Action(name_, description_)
{
}

void Spell::addComponent(std::unique_ptr<Components> component)
{
    components.push_back(std::move(component));
}

bool Spell::cast(Game& game, Character& player)
{
    Observer context(player, game);
    context.name = name;
    context.type = ActionType::SPELL;

    // Sort components by execution priority
    std::sort(components.begin(), components.end(),
              [](const std::unique_ptr<Components>& a, const std::unique_ptr<Components>& b)
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

            // Stop execution if spell failed and component is not optional
            if (context.states.game.has(GameCondition::SpellFailed) && !component->isOptional)
            {
                std::cout << "Spell failed: " << context.failureReason << std::endl;
                break;
            }
        }
    }
    return true;
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
    effect->primaryEffects.push_back(primaryFX);
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

void Fireball::operator()(Game& game, Character& player) { cast(game, player); }
