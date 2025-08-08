#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include "character.h"
#include "action.h"

class Player;
class Enemy;

class Spell : public Action{

    public:

    Spell(const std::string& name_, const std::string& description_) : 
    Action(name_,description_)
    {}

    
    virtual void cast(std::vector<Enemy>& enemies, std::vector<Player>& allies, Character& caster) = 0;
    

};

#endif