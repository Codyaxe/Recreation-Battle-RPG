#ifndef DEBUFFS_H
#define DEBUFFS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Debuff : public Action{

    public:

    Debuff(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void afflict(std::vector<Enemy>& enemies, std::vector<Player>& allies, Player& player) = 0;
    

};

#endif