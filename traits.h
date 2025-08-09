#ifndef TRAITS_H
#define TRAITS_H

#include "action.h"
#include <string>
#include <vector>

class Character;
class Player;
class Enemy;

class Trait : public Action
{

  public:
    Trait(const std::string& name_, const std::string& description_) : Action(name_, description_)
    {
    }

    virtual void exhibit(Game& game, Player& player) = 0;
};

#endif