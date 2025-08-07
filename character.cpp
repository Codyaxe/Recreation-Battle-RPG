#include "character.h"


// class Character{

//     public:
//     std::vector<Spell*> spellInv;
//     std::vector<Attack*> attackInv;
//     std::vector<Summon*> summonInv;
//     std::vector<Item*> itemInv;
//     std::vector<Debuff*> debuffInv;
//     std::vector<Buff*> buffInv;
//     std::vector<Trait*> traitInv;
//     std::string name;
//     int health;
//     int defense;
//     int speed;
//     int accuracy;

//     int mana;
//     int resistance;
//     int weakness;

    

// };

Character::Character(std::string name_, int health_, int defense_, int speed_, 
                    int accuracy_, int mana_, int resistance_, int weakness_) :

name(name_), health(health_), defense(defense_), speed(speed_), accuracy(accuracy_), 
mana(mana_), resistance(resistance_), weakness(weakness_)

{}

Player::Player() : 
Character("Generic Player", 1000, 50, 100, 100, 0, 0, 0)
{}

Player::Player(std::string name_, int health_, int defense_, int speed_, 
                int accuracy_, int mana_, int resistance_, int weakness_, 
                int barrier_, int shield_, int armor_) :

Character(name_, health_, defense_, speed_, accuracy_, mana_, resistance_, weakness_), 
barrier(barrier_), shield(shield_), armor(armor_)

{}

Enemy::Enemy() : 
Character("Generic", 200, 50, 100, 100, 0, 0, 0)
{}