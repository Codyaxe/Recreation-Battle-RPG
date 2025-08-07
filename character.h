#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <string>
#include "spells.h"

class Attack;
class Summon;
class Item;
class Debuff;
class Buff;
class Trait;


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

    int mana;
    int resistance;
    int weakness;

    Character(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_);

};

class Player : public Character{

    public:
    int barrier;
    int shield;
    int armor;

    Player();

    Player(std::string name_, int health_, int defense_, int speed_, int accuracy_, int mana_, 
    int resistance_, int weakness_, int barrier_, int shield_, int armor_);

};

class Enemy : public Character{

    public:

    Enemy();
};


#endif