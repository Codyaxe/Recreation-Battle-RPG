#ifndef BUFFS_H
#define BUFFS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Buff : public Action{

    public:

    Buff(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void buff(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif