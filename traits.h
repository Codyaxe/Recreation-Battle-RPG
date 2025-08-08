#ifndef TRAITS_H
#define TRAITS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Trait : public Action{

    public:

    Trait(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void exhibit(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif