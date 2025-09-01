#pragma once

#include "actions/action.h"

class Character;
class Player;
class Enemy;

class Item : public Action
{

  public:
    Item(const std::string& name_, const std::string& description_);
    ~Item() = default;

    virtual Return_Flags use(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Item> clone() const
    {
        return std::make_unique<Item>(name, description);
    };
};
