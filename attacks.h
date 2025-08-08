#ifndef ATTACKS_H
#define ATTACKS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Attack : public Action{

    public:

    Attack(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void initiate(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif