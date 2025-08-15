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
    TargetCondition condition;
    int duration;
    int strength = 0;
    Status(const std::string& name_, const std::string& description_);
    ~Status() = default;

    virtual Return_Flags trigger(Game& game, Character* player);
    virtual Return_Flags expire(Game& game, Character* player);
    // virtual bool expiration(Game& game, Character* player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Status> clone() const
    {
        return std::make_unique<Status>(name, description);
    };
};

class Poison : public Status
{
  public:
    Poison();
    Poison(int dur, int str);
    ~Poison() = default;

    std::unique_ptr<Status> clone() const override
    {
        return std::make_unique<Poison>(duration, strength);
    };
};

#endif