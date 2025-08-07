#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>
#include "spells.h"

class Attack;
class Summon;
class Item;
class Debuff;
class Buff;
class Trait;
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
    std::vector<Spell*> spellInv;
    std::vector<Attack*> attackInv;
    std::vector<Summon*> summonInv;
    std::vector<Item*> itemInv;
    std::vector<Debuff*> debuffInv;
    std::vector<Buff*> buffInv;
    std::vector<Trait*> traitInv;
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

    void attack(Game& game);
    void defend(Game& game);
    void spell(Game& game);
    void item(Game& game);
    void summon(Game& game);


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

};

class Enemy : public Character{

    public:

    Enemy();
};


#endif