#ifndef SUMMONS_H
#define SUMMONS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Summon : public Action{

    public:

    Summon(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void call(std::vector<Enemy>& enemies, std::vector<Player>& allies, Player& player) = 0;
    

};

#endif