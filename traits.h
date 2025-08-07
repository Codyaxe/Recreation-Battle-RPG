#ifndef TRAITS_H
#define TRAITS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Trait : public Action{

    public:

    Trait(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void effect(std::vector<Enemy>& targetInv, Player& player) = 0;
    

};

#endif