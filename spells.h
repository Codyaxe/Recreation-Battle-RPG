#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include <vector>
#include "action.h"

class Character;
class Player;
class Enemy;

class Spell : public Action{

    public:

    Spell(const std::string& name_, const std::string& description_);

    
    virtual void cast(std::vector<Character*>& enemies,std::vector<Character*>& allies, Character& player) = 0;

};

class Fireball : public Spell {

    public:

    Fireball();

    void cast(std::vector<Character*>& enemies, std::vector<Character*>& allies, Character& player) override;

    void operator()(std::vector<Character*>& enemy, std::vector<Character*>& allies, Character& player) override;
};

#endif