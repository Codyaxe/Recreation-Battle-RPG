#ifndef SUMMONS_H
#define SUMMONS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Summon : public Action{

    public:

    Summon(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void call(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif