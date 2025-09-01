#pragma once
#include "actions/action.h"

class Character;
class Player;
class Enemy;

class Summon : public Action
{

  public:
    Summon(const std::string& name_, const std::string& description_);
    ~Summon() = default;

    virtual Return_Flags call(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Summon> clone() const
    {
        return std::make_unique<Summon>(name, description);
    };
};
