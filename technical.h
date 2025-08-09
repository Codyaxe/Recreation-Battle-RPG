#ifndef TECHNICAL_H
#define TECHNICAL_H

#include "components.h"
#include <iostream>
#include <vector>
#include <windows.h>

class Character;
class Player;
class Enemy;
class Action;

const std::string RESET = "\033[0m";
const std::string HIGHLIGHT = "\033[47;30m";

// Will Use Scoped Enums

enum Target_Group
{
    ENEMIES = 0,
    ALLIES = 1,
    BOTH = 2,
    NONE = 3
};

enum Return_Flags
{
    EXIT = -1,
    SKIP = -2,
    END = -3
};

enum Action_Result
{
    RESULT_NONE = 0,
    END_TURN = -1,
    SKIP_TURN = -2,
    END_BATTLE = -5,
    CONTINUE_TURN = 1,
    EXTRA_TURN = 2,
    INTERRUPT_TURN = 3
};

void clearScreen();

class Game
{
  public:
    std::vector<Character*> enemies;
    std::vector<Character*> allies;
    BitsetWrapper<GameCondition> gameConditions;

    Game();
    ~Game();
};

void displayMenu(int selectedIndex, Player& player, bool resetDisplay = true);
void displayTargetMenu(int iter, int selectedIndex, std::vector<Character*>& targets,
                       Observer& context);
void displayChooseMenu(int selectedIndex, std::vector<Action*>& inv, const ActionType& type);
void displayPlayerSelect(int selectedIndex, std::vector<Player*>& allies);
int menu(Game& game, Player& player);
Action* menuChoose(Character& player, const ActionType& type);
Action* menuChooseHelper(std::vector<Action*>& inv, const ActionType& type);
int menuTarget(int iter, std::vector<Character*>& targets, Observer& context);
int selectPlayer(std::vector<Player*>& allies);
void menuPlayer(Game& game);
bool chooseTarget(Observer& context, TargetingComponent& targetingComponent);

class Interface
{

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