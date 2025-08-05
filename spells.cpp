

#include <string>
#include <functional>

class Character;

using EffectFunction = std::function<void(Character&)>;

class Spell{

    private:

    std::string name_;
    std::string description_;
    EffectFunction effect_;

    public:

    Spell(std::string& name, std::string& description, EffectFunction effect) : 
    name_(name), description_(description), effect_(effect)
    {}

    void cast(Character& target);

};

