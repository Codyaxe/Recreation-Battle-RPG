#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include <functional>

class Character;

using EffectFunction = std::function<void(Character&)>;

class Spell{

    private:

    std::string name_;
    std::string description_;
    std::vector<EffectFunction> effect_;

    public:

    Spell(std::string& name, std::string& description, std::vector<EffectFunction> effect);

    virtual void cast(Character& target) = 0;

};

#endif