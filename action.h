#ifndef ACTION_H
#define ACTION_H
#include <string>
#include "character.h"

class Enemy;
class Player;

class Action {

    public:
    std::string name;
    std::string description;

    Action();
    Action(std::string name_, std::string description_);
    virtual void operator()(std::vector<Enemy>& targetInv, Player& player) = 0;

};

#endif