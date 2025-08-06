#ifndef CHARACTER_H
#define CHARACTER_H

#include <map>
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

};

class Player : public Character{

    public:
    int shield;
    int barrier;
    int armor;

};

class Enemy : public Character{


};


#endif