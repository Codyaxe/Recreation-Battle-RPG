#pragma once

#include "actions/action.h"
#include <string>
#include <vector>
#include <memory>

class Spell : public Action
{
  public:
    Spell(const std::string& name_, const std::string& description_);
    ~Spell() = default;

    void addComponent(std::unique_ptr<Component> component);

    virtual Return_Flags cast(Game& game, Character& player);
    virtual std::unique_ptr<Spell> clone() const
    {
        return std::make_unique<Spell>(name, description);
    };
};

class Fireball : public Spell
{

  public:
    Fireball();
    ~Fireball() = default;

    std::unique_ptr<Spell> clone() const override { return std::make_unique<Fireball>(); };
};

class Poison_Gas : public Spell
{

  public:
    Poison_Gas();
    ~Poison_Gas() = default;

    std::unique_ptr<Spell> clone() const override { return std::make_unique<Poison_Gas>(); };
};
