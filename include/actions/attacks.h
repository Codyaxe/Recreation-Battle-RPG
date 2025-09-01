#pragma once

#include "actions/action.h"

class Character;
class Player;
class Enemy;

class Attack : public Action
{

  public:
    Attack(const std::string& name_, const std::string& description_);
    ~Attack() = default;

    virtual Return_Flags initiate(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Attack> clone() const
    {
        return std::make_unique<Attack>(name, description);
    };
};
