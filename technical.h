#ifndef TECHNICAL_H
#define TECHNICAL_H

#include <iostream>
#include <vector>
#include <windows.h>

class Character;
class Player;
class Enemy;
class Action;

const std::string RESET = "\033[0m";
const std::string HIGHLIGHT = "\033[47;30m";

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

enum Target_Group{
    ENEMIES = 50,
    ALLIES = 51,
    BOTH = 52,
};

enum Return_Flags{
    EXIT = -1,
    SKIP = -2,
    END = -3
};

enum Action_Result{
    NONE = 0,
    END_TURN = -1,
    SKIP_TURN = -2,
    END_BATTLE= -5,
    CONTINUE_TURN = 1,
    EXTRA_TURN = 2,    
    INTERRUPT_TURN = 3
};

void clearScreen();

class Game{
    public:

    std::vector<Character*> enemies;
    std::vector<Character*> allies;

    Game();
    ~Game();
};

void displayMenu(int selectedIndex, Player& player, bool resetDisplay = true);
void displayTargetMenu(int selectedIndex, std::vector<Character*>& targets, const std::string& name, const int& type);
void displayChooseMenu(int selectedIndex, std::vector<Action*>& inv, const int& type);
void displayPlayerSelect(int selectedIndex, std::vector<Player*>& allies);
int menu(Game& game, Player& player);
Action* menuChoose(Character& player, const int& type);
Action* menuChooseHelper(std::vector<Action*>& inv,const int& type);
int menuTarget(std::vector<Character*>& targets, const std::string& name, const int& type);
int selectPlayer(std::vector<Player*>& allies);
void menuPlayer(Game& game);
std::vector<int> chooseTarget(std::vector<Character*>& enemies, std::vector<Character*>& allies, std::string& name, int numberofTargets, int groupTarget, int type);


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