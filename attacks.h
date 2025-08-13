#ifndef ATTACKS_H
#define ATTACKS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Attack : public Action
{

  public:
    Attack(const std::string& name_, const std::string& description_);
    ~Attack() = default;

    virtual bool initiate(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Action> clone() const override
    {
        return std::make_unique<Attack>(name, description);
    };
};

#endif