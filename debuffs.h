#ifndef DEBUFFS_H
#define DEBUFFS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Debuff : public Action{

    public:

    Debuff(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void afflict(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif