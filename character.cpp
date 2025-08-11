#include "character.h"
#include "spells.h"
#include "technical.h"

// class Character{

//     public:
//     std::vector<Spell*> spellInv;
//     std::vector<Attack*> attackInv;
//     std::vector<Summon*> summonInv;
//     std::vector<Item*> itemInv;
//     std::vector<Debuff*> debuffInv;
//     std::vector<Buff*> buffInv;
//     std::vector<Trait*> traitInv;
//     std::string name;
//     int health;
//     int defense;
//     int speed;
//     int accuracy;

//     int mana;
//     int resistance;
//     int weakness;

// };

Character::Character(std::string name_, std::string element_, int health_, int defense_, int power_,
                     int magic_, int speed_, int accuracy_, int mana_, int resistance_,
                     int weakness_)
    :

      name(name_), element(element_), health(health_), defense(defense_), power(power_),
      magic(magic_), speed(speed_), accuracy(accuracy_), mana(mana_), resistance(resistance_),
      weakness(weakness_)

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

// Name, Element, HP, Defense, AT, Magic, Speed, Acc, Mana, Resistance, Weakness

Player::Player() : Character("Generic Player", "Normal", 200, 50, 100, 100, 100, 100, 100, 0, 0)
{
    initActions();
}

Player::Player(std::string name_)
    : Character(name_, "Normal", 200, 50, 100, 100, 100, 100, 100, 0, 0)
{
    initActions();
}

Player::Player(std::string name_, std::string element_, int health_, int defense_, int power_,
               int magic_, int speed_, int accuracy_, int mana_, int resistance_, int weakness_,
               int barrier_, int shield_, int armor_)
    :

      Character(name_, element_, health_, defense_, power_, magic_, speed_, accuracy_, mana_,
                resistance_, weakness_),
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
            {"Skip", [this](Game& game) { return skip(game); }}};
}

int Player::attack(Game& game)
{
    Sleep(1000);
    return END_TURN; // Placeholder
};

int Player::defend(Game& game)
{
    Sleep(1000);
    return END_TURN; // Placeholder
};

int Player::spell(Game& game)
{
    Action* action = menuChoose(*this, ActionType::SPELL);
    Spell* spell = dynamic_cast<Spell*>(action);
    if (spell == nullptr)
    {
        return CONTINUE_TURN;
    }
    if (!spell->cast(game, *this))
    {
        return CONTINUE_TURN;
    };
    return END_TURN; // Placeholder
};

int Player::item(Game& game)
{
    Sleep(1000);
    return CONTINUE_TURN;
};

int Player::summon(Game& game)
{
    Sleep(1000);
    return END_TURN; // Placeholder
};

int Player::skip(Game& game)
{
    Sleep(1000);
    return SKIP_TURN;
};

Enemy::Enemy() : Character("Generic Enemy", "Nature", 200, 50, 100, 100, 100, 100, 100, 0, 0) {}

int Enemy::attack(Game& game)
{
    return END_TURN; // Placeholder
};

int Enemy::defend(Game& game)
{
    return END_TURN; // Placeholder
};

int Enemy::spell(Game& game)
{
    return END_TURN; // Placeholder
};

int Enemy::item(Game& game)
{
    return END_TURN; // Placeholder
};

int Enemy::summon(Game& game)
{
    return END_TURN; // Placeholder
};

int Enemy::skip(Game& game)
{
    return END_TURN; // Placeholder
};

std::vector<Character::MenuAction> Enemy::getMenuActions()
{
    return {{"None", [this](Game& game) { return spell(game); }}};
}
