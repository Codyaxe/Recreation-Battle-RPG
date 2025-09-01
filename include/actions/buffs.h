#pragma once

#include "actions/action.h"

class Character;
class Player;
class Enemy;

class Buff : public Action
{

  public:
    Buff(const std::string& name_, const std::string& description_);
    ~Buff() = default;

    virtual Return_Flags buff(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Buff> clone() const
    {
        return std::make_unique<Buff>(name, description);
    };
};
