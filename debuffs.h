#ifndef DEBUFFS_H
#define DEBUFFS_H

#include "action.h"
#include <string>
#include <vector>

class Character;
class Player;
class Enemy;

class Debuff : public Action
{

  public:
    Debuff(const std::string& name_, const std::string& description_) : Action(name_, description_)
    {
    }

    virtual void afflict(Game& game, Player& player) = 0;
};

#endif