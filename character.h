#ifndef CHARACTER_H
#define CHARACTER_H

#include "components.h"
#include <bitset>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class Action;
class Game;

class Character
{

  public:
    struct MenuAction
    {
        std::string name;
        std::function<int(Game& game)> execute;
    };

    std::vector<MenuAction> commands;
    int commandsSize;

    std::vector<Action*> spellInv;
    std::vector<Action*> attackInv;
    std::vector<Action*> summonInv;
    std::vector<Action*> itemInv;
    std::vector<Action*> debuffInv;
    std::vector<Action*> buffInv;
    std::vector<Action*> traitInv;
    std::string name;
    std::string element;
    int baseHealth;
    int health;
    int defense;
    int power;
    int magic;
    int speed;
    int accuracy;
    int mana;
    int resistance;
    int weakness;
    int luck; // For Critical Hit Chance

    // A Bitmask That Will Handle State Flags
    BitsetWrapper<TargetCondition> targetConditions;
    // For Filtering if a Character has an On X Ablity
    BitsetWrapper<OnEventConditions> onEventsAbilities;

    Character(std::string name_, std::string element_, int health_, int defense_, int power_,
              int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
              int luck_);
    virtual ~Character() = default;
    virtual int attack(Game& game) = 0;
    virtual int defend(Game& game) = 0;
    virtual int item(Game& game) = 0;
    virtual int skip(Game& game) = 0;
    virtual int spell(Game& game) = 0;
    virtual int summon(Game& game) = 0;

    // Non-Pure Virtual On-Event
    virtual void onDeath(Game& game) {}
    virtual void onSummon(Game& game) {}
    virtual void onInitiate(Game& game) {}
    virtual void onCast(Game& game) {}
    virtual void onAfflict(Game& game) {}
    virtual void onBuff(Game& game) {}
    virtual void onExhibit(Game& game) {}
    virtual void onCall(Game& game) {}
    virtual void onUse(Game& game) {}
    virtual void onChange(Game& game) {}
    virtual void onPlay(Game& game) {}
    virtual void onDamageTaken(Game& game) {}
    virtual void onDealingDamage(Game& game) {}
    virtual void onHeal(Game& game) {}
    virtual void onGainX(Game& game) {}
    virtual void onLoseX(Game& game) {}
    virtual void onStartTurn(Game& game) {}
    virtual void onEndTurn(Game& game) {}
    virtual void onCrit(Game& game) {}
    virtual void onBlock(Game& game) {}
    virtual void onParry(Game& game) {}
    virtual void onDodge(Game& game) {}
    virtual void onMiss(Game& game) {}
    virtual void onKill(Game& game) {}
    virtual void onTakingFatalDamage(Game& game) {}

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
    Player(std::string name_);

    Player(std::string name_, std::string element_, int health_, int defense_, int power_,
           int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
           int luck_, int barrier_, int shield_, int armor_);

    std::vector<Character::MenuAction> getMenuActions() override;
    int attack(Game& game) override;
    int defend(Game& game) override;
    int spell(Game& game) override;
    int item(Game& game) override;
    int skip(Game& game) override;
    int summon(Game& game) override;
};

class Enemy : public Character
{

  public:
    Enemy();

    std::vector<Character::MenuAction> getMenuActions() override;
    int attack(Game& game) override;
    int defend(Game& game) override;
    int spell(Game& game) override;
    int item(Game& game) override;
    int skip(Game& game) override;
    int summon(Game& game) override;
};

#endif