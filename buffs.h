#ifndef BUFFS_H
#define BUFFS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Buff : public Action
{

  public:
    Buff(const std::string& name_, const std::string& description_);
    ~Buff() = default;

    virtual bool buff(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Action> clone() const override
    {
        return std::make_unique<Buff>(name, description);
    };
};

#endif