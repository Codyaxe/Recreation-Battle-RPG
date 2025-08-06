#ifndef TECHNICAL_H
#define TECHNICAL_H

#include <iostream>
#include <vector>
#include <windows.h>
#include "character.h"

const std::string RESET;
const std::string HIGHLIGHT;
const std::string actions[5];

const int NUM_ACTIONS;

enum Target_Type{
    SPELL = 10,
    ATTACK = 11,
    SUMMON = 12,
    ITEM = 13,
    DEBUFF = 14,
    BUFF = 15,
    TRAIT = 16
};

enum Exit_Flag{
    EXIT = -1,
};

void clearScreen();

class Game{
    public:

    std::vector<Enemy> enemyField;
    Player player;

    Game();
    ~Game();

    void attack();
    void defend();
    void spell();
    void item();
    void summon();
};

void displayMenu(int selectedIndex, bool resetDisplay = true);
void displayTargetMenu(int selectedIndex, std::vector<Character>& targetInv, const std::string& name, const int& type);
void menu(Game& game);
int menuTarget(std::vector<Character>& targetInv, const std::string& name, const int& type);
std::vector<int> chooseTarget(std::vector<Character>& targetInv, std::string& name, int numberofTargets, bool targetPlayer = false);

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
    friend void menu(const Interface& interface_);



};



#endif