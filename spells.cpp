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

    Fireball(std::string& name, std::string& description) : 
    Spell(name, description)
    {}

    void effect(std::vector<Enemy>& targetInv) override {
        std::vector<int> targets = chooseTarget(targetInv, name_, 1);
        applyDamage(targetInv[targets[0]], 150, FIRE);
    }

    void operator()(std::vector<Enemy>& targetInv) override {
        effect(targetInv);
    }

};
