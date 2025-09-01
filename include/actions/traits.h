#pragma once

#include "actions/action.h"

class Character;
class Player;
class Enemy;

class Trait : public Action
{

  public:
    Trait(const std::string& name_, const std::string& description_);
    ~Trait() = default;

    virtual Return_Flags exhibit(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Trait> clone() const
    {
        return std::make_unique<Trait>(name, description);
    };
};
