#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include <functional>
#include "character.h"

using EffectFunction = std::function<void(Character&)>;

class Spell{

    public:

    std::string name;
    std::string description;

    Spell(const std::string& name_, const std::string& description_) : 
    name(name_), description(description_)
    {}

    
    virtual void effect(std::vector<Enemy>& targetInv, Player& player) = 0;
    virtual void operator()(std::vector<Enemy>& targetInv, Player& player) = 0;

};

#endif