#include "spells.h"
#include "character.h"
#include "spelleffects.h"
#include "technical.h"


// class Spell{

//     private:

//     std::string name_;
//     std::string description_;
//     EffectFunction effect_;

//     

//     Spell(std::string& name, std::string& description) : 
//     name_(name), description_(description), effect_(effect)
//     {}

//     virtual void effect(Character& target) = 0;

// };

    Spell::Spell(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    Fireball::Fireball() : 
    Spell("Fireball", "A fiery blast against an enemy.")
    {}
    
    void Fireball::cast(std::vector<Character*>& enemies, std::vector<Character*>& allies, Character& player) {
        std::vector<int> targets = chooseTarget(enemies, allies, name, 1, ENEMIES, SPELL);
        applyDamage(enemies[targets[0]], 150, FIRE);
    }

    void Fireball::operator()(std::vector<Character*>& enemy, std::vector<Character*>& allies, Character& player) {
        cast(enemy, allies, player);
    }
 
