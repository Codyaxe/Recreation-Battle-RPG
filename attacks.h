#ifndef ATTACKS_H
#define ATTACKS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Attack : public Action{

    public:

    Attack(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void initiate(std::vector<Enemy>& enemies, std::vector<Player>& allies, Player& player) = 0;
    

};

#endif