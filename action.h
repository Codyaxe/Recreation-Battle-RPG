#ifndef ACTION_H
#define ACTION_H
#include <string>
#include <vector>
#include <memory>

class Character;
class Enemy;
class Player;
class Game;

class Action
{

  public:
    std::string name;
    std::string description;

    Action();
    Action(const std::string& name_, const std::string& description_);

    ~Action() = default;
    virtual std::unique_ptr<Action> clone() const = 0;
};

#endif