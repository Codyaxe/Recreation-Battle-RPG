#ifndef ACTION_H
#define ACTION_H
#include "components.h"
#include "return_flags.h"
#include "technical.h"
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

#endif