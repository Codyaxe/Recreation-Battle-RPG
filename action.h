#ifndef ACTION_H
#define ACTION_H
#include <string>
#include "character.h"

class Enemy;
class Player;
class Character;

class Action {

    public:
    std::string name;
    std::string description;

    Action();
    Action(std::string name_, std::string description_);
    virtual void operator()(std::vector<Enemy>& targetInv, Character& player) = 0;

};

#endif