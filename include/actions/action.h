#pragma once
#include "core/components.h"
#include "core/return_flags.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

class Character;
class Enemy;
class Player;
class Game;

class Action
{

  public:
    std::string name;
    std::string description;
    std::vector<std::unique_ptr<Component>> components;

    Action();
    Action(const std::string& name_, const std::string& description_);

    virtual ~Action() = default;

    void addComponent(std::unique_ptr<Component> component);
};
