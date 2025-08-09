#ifndef ACTION_H
#define ACTION_H
#include <string>
#include <vector>

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
    virtual void operator()(Game& game, Character& player) = 0;
};

#endif