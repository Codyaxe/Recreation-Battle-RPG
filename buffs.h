#ifndef BUFFS_H
#define BUFFS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
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