#ifndef TRAITS_H
#define TRAITS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Trait : public Action
{

  public:
    Trait(const std::string& name_, const std::string& description_);
    ~Trait() = default;

    virtual bool exhibit(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Action> clone() const override
    {
        return std::make_unique<Trait>(name, description);
    };
};
#endif