#ifndef CHARACTER_H
#define CHARACTER_H

#include <map>
#include <string>

class Spell;
class Attack;
class Summon;
class Item;


class Character{

    public:
    std::map<int,Spell> spellInv;
    std::map<int,Attack> attackInv;
    std::map<int,Summon> summonInv;
    std::map<int,Item> itemInv;
    std::string name;
    int health;
    int mana;

    virtual void cast(Spell& spell) = 0;

};

class Player : Character{



};

class Enemy : Character{


};


#endif