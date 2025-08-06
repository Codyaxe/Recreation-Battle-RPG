#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include <functional>
#include "character.h"

using EffectFunction = std::function<void(Character&)>;

class Spell{

    public:

    std::string name_;
    std::string description_;

    Spell(std::string& name, std::string& description) : 
    name_(name), description_(description)
    {}

    
    virtual void effect(std::vector<Enemy>& targetInv) = 0;
    virtual void operator()(std::vector<Enemy>& targetInv) = 0;

};

#endif