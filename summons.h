#ifndef SUMMONS_H
#define SUMMONS_H

#include "action.h"
#include <string>
#include <vector>


class Character;
class Player;
class Enemy;

class Summon : public Action
{

  public:
    Summon(const std::string& name_, const std::string& description_) : Action(name_, description_)
    {
    }

    virtual void call(Game& game, Player& player) = 0;
};

#endif