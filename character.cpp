#include "character.h"
#include "technical.h"


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

void Character::setState(State flag) {
    states.set(static_cast<size_t>(flag));
}

void Character::clearState(State flag) {
    states.reset(static_cast<size_t>(flag));
}

bool Character::hasState(State flag) const {
    return states.test(static_cast<size_t>(flag));
}

void Character::attack(Game& game){

};

void Character::defend(Game& game){

};

void Character::spell(Game& game){
    Action* action = menuChoose(*this, SPELL);
    Spell* spell = dynamic_cast<Spell*>(action);
    if (spell == nullptr){
        return;
    }
    spell->cast(game.enemyField, game.playerField, *this);
};

void Character::item(Game& game){

};

void Character::summon(Game& game){

};


Player::Player() : 
Character("Generic Player", 1000, 50, 100, 100, 0, 0, 0)
{}

Player::Player(std::string name_) : 
Character(name_, 200, 50, 100, 100, 0, 0, 0)
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

