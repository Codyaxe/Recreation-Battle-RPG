#ifndef BUFFS_H
#define BUFFS_H

#include "action.h"
#include <string>
#include <vector>

class Character;
class Player;
class Enemy;

class Buff : public Action
{

  public:
    Buff(const std::string& name_, const std::string& description_) : Action(name_, description_) {}

    virtual void buff(Game& game, Player& player) = 0;
};

#endif