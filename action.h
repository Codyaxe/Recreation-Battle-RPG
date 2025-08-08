#ifndef ACTION_H
#define ACTION_H
#include <string>
#include <vector>

class Character;
class Enemy;
class Player;


class Action {

    public:
    std::string name;
    std::string description;

    Action();
    Action(const std::string& name_, const std::string& description_);
    virtual void operator()(std::vector<Character*>& enemies, std::vector<Character*>& allies, Character& player) = 0;

};

#endif