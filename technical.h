#ifndef TECHNICAL_H
#define TECHNICAL_H

#include <iostream>
#include <vector>
#include <windows.h>
#include "character.h"

const std::string RESET = "\033[0m";
const std::string HIGHLIGHT = "\033[47;30m";
const std::string actions[6] = { "Attack", "Defend", "Spell", "Item", "Summon", "Skip" };
const int NUM_ACTIONS = sizeof(actions) / sizeof(actions[0]);

//Will Use Scoped Enums
enum Target_Type{
    SPELL = 10,
    ATTACK = 11,
    SUMMON = 12,
    ITEM = 13,
    DEBUFF = 14,
    BUFF = 15,
    TRAIT = 16
};

enum Return_Flags{
    EXIT = -1,
    SKIP = -2
};

void clearScreen();

class Game{
    public:

    std::vector<Enemy> enemyField;
    std::vector<Player> playerField;

    Game();
    ~Game();
};

void displayMenu(int selectedIndex, bool resetDisplay = true);
void displayTargetMenu(int selectedIndex, std::vector<Character>& targetInv, const std::string& name, const int& type);
template <typename T>
void displayChooseMenu(int selectedIndex, std::vector<T*>& inv, const int& type);
void displayPlayerSelect(int selectedIndex, std::vector<Player*>& playerInv);
int menu(Game& game, Player& player);
Action* menuChoose(Character& player, const int& type);
template <typename T>
T* menuChooseHelper(std::vector<T*>& inv,const int& type);
int menuTarget(std::vector<Character>& targetInv, const std::string& name, const int& type);
int selectPlayer(std::vector<Player*>& playerInv);
void menuPlayer(Game& game);
std::vector<int> chooseTarget(std::vector<Enemy>& targetInv, std::string& name, int numberofTargets, bool targetPlayer = false);


class Interface{

    public:

    static HANDLE hOut;
    static HANDLE hIn;
    DWORD originalOutMode, originalInMode; 
    bool enableFlags();
    void restoreConsoleMode();

    public:

    Interface();
    ~Interface();
    void start();

};



#endif