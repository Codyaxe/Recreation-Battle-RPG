#ifndef CHARACTER_H
#define CHARACTER_H

#include <map>
#include <memory>
#include <string>

class Spell;
class Attack;
class Summon;
class Item;
class Debuff;
class Buff;


class Character{

    public:
    std::map<int,Spell*> spellInv;
    std::map<int,Attack*> attackInv;
    std::map<int,Summon*> summonInv;
    std::map<int,Item*> itemInv;
    std::map<int,Debuff*> debuffInv;
    std::map<int,Buff*> buffInv;
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