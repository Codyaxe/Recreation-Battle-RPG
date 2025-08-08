#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>
#include "spells.h"

class Action;
class Game;

//Bitmask
enum class State : size_t {

    None = 0,
    Alive = 1,
    Dead = 2,
    Stunned = 3,
    Count 

};

constexpr size_t flagCount = static_cast<size_t>(State::Count);

class Character{

    public:
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

    //A Bitmask That Will Handle State Flags
    std::bitset<flagCount> states;

    int mana;
    int resistance;
    int weakness;

    Character(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_);

    void setState(State flag);

    void clearState(State flag);

    bool hasState(State flag) const;

    virtual void attack(Game& game) = 0;
    virtual void defend(Game& game) = 0;
    virtual void spell(Game& game) = 0;
    virtual void item(Game& game) = 0;
    virtual void summon(Game& game) = 0;


};

class Player : public Character{

    public:
    int barrier;
    int shield;
    int armor;

    Player();
    Player(std::string name_);
    
    Player(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_, 
    int resistance_, int weakness_, int barrier_, int shield_, int armor_);

    void attack(Game& game) override;
    void defend(Game& game) override;
    void spell(Game& game) override;
    void item(Game& game) override;
    void summon(Game& game) override;

};

class Enemy : public Character{

    public:

    Enemy();

    void attack(Game& game) override;
    void defend(Game& game) override;
    void spell(Game& game) override;
    void item(Game& game) override;
    void summon(Game& game) override;

};


#endif