#ifndef STATUS_H
#define STATUS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Status : public Action
{

  public:
    std::vector<EventCondition> activationConditions;
    int duration;
    int strength = 0;
    Status(const std::string& name_, const std::string& description_);
    ~Status() = default;

    virtual bool trigger(Game& game, Character* player);
    // virtual bool expiration(Game& game, Character* player);

    void addComponent(std::unique_ptr<Component> component);

    std::unique_ptr<Action> clone() const override
    {
        return std::make_unique<Status>(name, description);
    };
};

class Poison : public Status
{
  public:
    Poison();
    ~Poison() = default;

    std::unique_ptr<Action> clone() const override { return std::make_unique<Poison>(); };
};

#endif