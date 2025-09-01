#pragma once
#include "core/components.h"
#include "core/status.h"
#include "actions/traits.h"
#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class Action;
class Game;
enum class Action_Result;

class Character
{

  public:
    struct MenuAction
    {
        std::string name;
        std::function<Action_Result(Game& game)> execute;
    };

    std::vector<MenuAction> commands;
    int commandsSize;

    std::vector<std::unique_ptr<Action>> spellInv;
    std::vector<std::unique_ptr<Action>> attackInv;
    std::vector<std::unique_ptr<Action>> itemInv;
    // Special Actions
    std::vector<std::unique_ptr<Action>> summonInv;
    std::vector<std::unique_ptr<Action>> debuffInv;
    std::vector<std::unique_ptr<Action>> buffInv;
    std::vector<std::unique_ptr<Action>> traitInv;

    std::vector<std::unique_ptr<Status>> statuses;
    std::vector<std::unique_ptr<Trait>> acquiredTraits;
    std::string name;
    std::string element;
    int baseHealth;
    int health;
    int defense;
    int power;
    int magic;
    int speed;
    int accuracy;
    int baseMana;
    int mana;
    int resistance;
    int weakness;
    int luck; // For Critical Hit Chance
    // Status Based HP
    int absorption = 0;

    // A Bitmask That Will If The Character Has X Trait
    BitsetWrapper<TraitCondition> traitConditions;
    // A Bitmask That Will Handle State Flags
    BitsetWrapper<TargetCondition> targetConditions;
    // For Filtering if a Character has an On X Ablity
    BitsetWrapper<EventCondition> onEventsAbilities;

    Character() = default;
    Character(std::string name_, std::string element_, int health_, int defense_, int power_,
              int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
              int luck_);
    virtual ~Character() = default;
    virtual Action_Result attack(Game& game) = 0;
    virtual Action_Result defend(Game& game) = 0;
    virtual Action_Result item(Game& game) = 0;
    virtual Action_Result skip(Game& game) = 0;
    virtual Action_Result end(Game& game) = 0;
    virtual Action_Result spell(Game& game) = 0;
    virtual Action_Result summon(Game& game) = 0;

    // Non-Pure Virtual On-Event
    virtual void onDeath(Game& game) {}
    virtual void onSummon(Game& game) {}
    // On event when an attack executes;
    virtual void onInitiate(Game& game, std::string_view str) {}
    // On event when a spell executes;
    virtual void onCast(Game& game, std::string_view str) {}
    // On event when a debuff executes;
    virtual void onAfflict(Game& game, std::string_view str) {}
    // On event when a buff executes;
    virtual void onBuff(Game& game, std::string_view str) {}
    // On event when a trait executes;
    virtual void onExhibit(Game& game, std::string_view str) {}
    // On event when a summon executes;
    virtual void onCall(Game& game, std::string_view str) {}
    // On event when an item is utilized;
    virtual void onUse(Game& game, std::string_view str) {}
    // On event when equipment is utilized;
    virtual void onChange(Game& game, std::string_view str) {}
    // On event when a card is played (for Cardmasters);
    virtual void onPlay(Game& game, std::string_view str) {}
    virtual void onTargetInitiate(Game& game, std::string_view str) {}
    virtual void onTargetCast(Game& game, std::string_view str) {}
    virtual void onTargetAfflict(Game& game, std::string_view str) {}
    virtual void onTargetBuff(Game& game, std::string_view str) {}
    virtual void onTargetExhibit(Game& game, std::string_view str) {}
    virtual void onTargetCall(Game& game, std::string_view str) {}
    virtual void onTargetUse(Game& game, std::string_view str) {}
    virtual void onTargetChange(Game& game, std::string_view str) {}
    virtual void onTargetPlay(Game& game, std::string_view str) {}
    virtual void onTakingDamage(Game& game) {}
    virtual void onDealingDamage(Game& game) {}
    virtual void onHeal(Game& game) {}
    virtual void onGainX(Game& game, TargetCondition condition) {}
    virtual void onLoseX(Game& game, TargetCondition condition) {}
    virtual void onGainX(Game& game, TraitCondition condition) {}
    virtual void onLoseX(Game& game, TraitCondition condition) {}
    // Can add a condition X. If X happened, trigger Start and End Turn
    virtual void onCrit(Game& game) {}
    virtual void onBlock(Game& game) {}
    virtual void onParry(Game& game) {}
    virtual void onCounter(Game& game) {}
    virtual void onDodge(Game& game) {}
    virtual void onMiss(Game& game) {}
    virtual void onKill(Game& game) {}
    virtual void onTakingFatalDamage(Game& game) {}

    virtual void onTurnEnd(Game& game) {}
    virtual void onTurnStart(Game& game) {}

    virtual std::vector<MenuAction> getMenuActions();
    void initActions();
};

class Player : public Character
{

  public:
    int barrier;
    int shield;
    int armor;

    Player();
    explicit Player(std::string name_);

    Player(std::string name_, std::string element_, int health_, int defense_, int power_,
           int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
           int luck_, int barrier_, int shield_, int armor_);

    std::vector<Character::MenuAction> getMenuActions() override;
    Action_Result attack(Game& game) override;
    Action_Result defend(Game& game) override;
    Action_Result spell(Game& game) override;
    Action_Result item(Game& game) override;
    Action_Result skip(Game& game) override;
    Action_Result end(Game& game) override;
    Action_Result summon(Game& game) override;
};

class Enemy : public Character
{

  public:
    Enemy();
    explicit Enemy(std::string name_);

    std::vector<Character::MenuAction> getMenuActions() override;
    Action_Result attack(Game& game) override;
    Action_Result defend(Game& game) override;
    Action_Result spell(Game& game) override;
    Action_Result item(Game& game) override;
    Action_Result skip(Game& game) override;
    Action_Result end(Game& game) override;
    Action_Result summon(Game& game) override;
};
