#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Item : public Action{

    public:

    Item(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void use(std::vector<Character*>& enemies, std::vector<Character*>& allies, Player& player) = 0;
    

};

#endif