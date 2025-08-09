#ifndef ITEMS_H
#define ITEMS_H

#include "action.h"
#include <string>
#include <vector>

class Character;
class Player;
class Enemy;

class Item : public Action
{

  public:
    Item(const std::string& name_, const std::string& description_) : Action(name_, description_) {}

    virtual void use(Game& game, Player& player) = 0;
};

#endif