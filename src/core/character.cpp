#include "core/character.h"
#include "actions/spells.h"
#include "ui/technical.h"

Character::Character(std::string name_, std::string element_, int health_, int defense_, int power_,
                     int magic_, int speed_, int accuracy_, int mana_, int resistance_,
                     int weakness_, int luck_)
    :

      name(name_), element(element_), baseHealth(health_), health(health_), defense(defense_),
      power(power_), magic(magic_), speed(speed_), accuracy(accuracy_), mana(mana_),
      resistance(resistance_), weakness(weakness_), luck(luck_)

{
}

std::vector<Character::MenuAction> Character::getMenuActions()
{
    return {
        {"Attack", [this](Game& game) { return attack(game); }},
        {"Defend", [this](Game& game) { return defend(game); }},
        {"Item", [this](Game& game) { return item(game); }},
    };
}

void Character::initActions()
{
    commands = getMenuActions();
    commandsSize = commands.size();
}

// Name, Element, HP, Defense, AT, Magic, Speed, Acc, Mana, Resistance, Weakness, Luck

Player::Player() : Character("Generic Player", "Normal", 200, 50, 100, 100, 100, 100, 100, 0, 0, 1)
{
    initActions();
}

Player::Player(std::string name_)
    : Character(name_, "Normal", 200, 50, 100, 100, 100, 100, 100, 0, 0, 1)
{
    initActions();
}

Player::Player(std::string name_, std::string element_, int health_, int defense_, int power_,
               int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
               int luck_, int barrier_, int shield_, int armor_)
    :

      Character(name_, element_, health_, defense_, power_, magic_, speed_, accuracy_, mana_,
                resistance_, weakness_, luck_),
      barrier(barrier_), shield(shield_), armor(armor_)

{
    initActions();
}

std::vector<Character::MenuAction> Player::getMenuActions()
{
    return {{"Attack", [this](Game& game) { return attack(game); }},
            {"Defend", [this](Game& game) { return defend(game); }},
            {"Spell", [this](Game& game) { return spell(game); }},
            {"Item", [this](Game& game) { return item(game); }},
            {"Summon", [this](Game& game) { return summon(game); }},
            {"Skip", [this](Game& game) { return skip(game); }},
            {"End", [this](Game& game) { return end(game); }}};
}

Action_Result Player::attack(Game& game)
{
    Sleep(1000);
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Player::defend(Game& game)
{
    Sleep(1000);
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Player::spell(Game& game)
{
    Action* action = menuChoose(*this, ActionType::SPELL);
    Spell* spell = dynamic_cast<Spell*>(action);
    if (spell == nullptr)
    {
        return Action_Result::CONTINUE_TURN;
    }
    if (spell->cast(game, *this) == Return_Flags::EXIT)
    {
        return Action_Result::CONTINUE_TURN;
    };
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Player::item(Game& game)
{
    Sleep(1000);
    return Action_Result::CONTINUE_TURN;
};

Action_Result Player::summon(Game& game)
{
    Sleep(1000);
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Player::skip(Game& game)
{
    Sleep(1000);
    return Action_Result::SKIP_TURN;
};

Action_Result Player::end(Game& game)
{
    Sleep(1000);
    return Action_Result::END_BATTLE_TURN;
};

Enemy::Enemy() : Character("Generic Enemy", "Nature", 200, 50, 100, 100, 100, 100, 100, 0, 0, 1) {}
Enemy::Enemy(std::string name_)
    : Character(name_, "Nature", 200, 50, 100, 100, 100, 100, 100, 0, 0, 1)
{
}

Action_Result Enemy::attack(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::defend(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::spell(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::item(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::summon(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::skip(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

Action_Result Enemy::end(Game& game)
{
    return Action_Result::END_TURN; // Placeholder
};

std::vector<Character::MenuAction> Enemy::getMenuActions()
{
    return {{"None", [this](Game& game) { return spell(game); }}};
}
