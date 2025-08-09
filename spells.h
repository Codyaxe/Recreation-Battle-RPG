#ifndef SPELLS_H
#define SPELLS_H

#include "action.h"
#include "components.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class Spell : public Action
{
  public:
    std::vector<std::unique_ptr<Components>> components;

    Spell(const std::string& name_, const std::string& description_);

    void addComponent(std::unique_ptr<Components> component);

    virtual bool cast(Game& game, Character& player);
};

class Fireball : public Spell
{

  public:
    Fireball();

    void operator()(Game& game, Character& player) override;
};

#endif