#ifndef DEBUFFS_H
#define DEBUFFS_H

#include "action.h"

class Character;
class Player;
class Enemy;

class Debuff : public Action
{

  public:
    Debuff(const std::string& name_, const std::string& description_);
    ~Debuff() = default;

    virtual Return_Flags afflict(Game& game, Character& player);

    void addComponent(std::unique_ptr<Component> component);

    virtual std::unique_ptr<Debuff> clone() const
    {
        return std::make_unique<Debuff>(name, description);
    };
};

#endif