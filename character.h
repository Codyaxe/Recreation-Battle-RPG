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

// Bitmask
enum class State : size_t
{

    None = 0,
    Alive = 1,
    Dead = 2,
    Stunned = 3,
    Count

};

constexpr size_t flagCount = static_cast<size_t>(State::Count);

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
    int health;
    int defense;
    int speed;
    int accuracy;
    int mana;
    int resistance;
    int weakness;

    // A Bitmask That Will Handle State Flags
    BitsetWrapper<TargetCondition> targetConditions;

    Character(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_,
              int resistance_, int weakness_);

    virtual int attack(Game& game) = 0;
    virtual int defend(Game& game) = 0;
    virtual int item(Game& game) = 0;
    virtual int skip(Game& game) = 0;
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

    Player(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_,
           int resistance_, int weakness_, int barrier_, int shield_, int armor_);

    std::vector<Character::MenuAction> getMenuActions() override;
    int attack(Game& game) override;
    int defend(Game& game) override;
    virtual int spell(Game& game);
    int item(Game& game) override;
    int skip(Game& game) override;
    virtual int summon(Game& game);
};

class Enemy : public Character
{

  public:
    Enemy();

    std::vector<Character::MenuAction> getMenuActions() override;
    int attack(Game& game) override;
    int defend(Game& game) override;
    virtual int spell(Game& game);
    int item(Game& game) override;
    int skip(Game& game) override;
    virtual int summon(Game& game);
};

#endif