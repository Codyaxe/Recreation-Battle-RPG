#include "spells.h"
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

class Fireball : public Spell {

    Fireball(const std::string& name_, const std::string& description_) : 
    Spell(name_, description_)
    {}

    void cast(std::vector<Character*>& enemies, std::vector<Character*>& allies, Character& player) override {
        std::vector<int> targets = chooseTarget(enemies, allies, name, 1, ENEMIES, SPELL);
        applyDamage(enemies[targets[0]], 150, FIRE);
    }

    void operator()(std::vector<Character*>& enemy, std::vector<Character*>& allies, Character& player) override {
        cast(enemy, allies, player);
    }
 
};
