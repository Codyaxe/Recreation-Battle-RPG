#ifndef ITEMS_H
#define ITEMS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Item : public Action
{

  public:
    Item(const std::string& name_, const std::string& description_);
    ~Item() = default;

    virtual bool use(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Action> clone() const override
    {
        return std::make_unique<Item>(name, description);
    };
};

#endif