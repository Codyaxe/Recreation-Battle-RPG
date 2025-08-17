#ifndef TECHNICAL_H
#define TECHNICAL_H

#include "components.h"
#include <iostream>
#include <limits>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <windows.h>
#include "return_flags.h"

class Character;
class Player;
class Enemy;
class Action;
class BattleContext;

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

enum class Action_Result
{
    RESULT_NONE,
    END_TURN,
    SKIP_TURN,
    END_BATTLE_TURN,
    CONTINUE_TURN,
    EXTRA_TURN,
    INTERRUPT_TURN
};

void clearScreen();
bool processSkip(std::mutex& skipMutex, std::condition_variable& skipCv, bool& skipPressed,
                 bool& shouldExit);
class Game
{
  public:
    std::vector<std::unique_ptr<Character>> enemies;
    std::vector<std::unique_ptr<Character>> allies;
    BitsetWrapper<GameCondition> gameConditions;

    Game();
    ~Game();
};

void displayMenu(int selectedIndex, Player& player, bool resetDisplay = true);
void displayTargetMenu(int iter, int selectedIndex,
                       std::vector<std::unique_ptr<Character>>& targets, BattleContext& context);
void displayChooseMenu(int selectedIndex, std::vector<std::unique_ptr<Action>>& inv,
                       const ActionType& type);
void displayPlayerSelect(int selectedIndex, std::vector<Player*>& allies);
Return_Flags menu(Game& game, Player& player);
Action* menuChoose(Character& player, const ActionType& type);
Action* menuChooseHelper(std::vector<std::unique_ptr<Action>>& inv, const ActionType& type);
Return_Flags menuTarget(int iter, std::vector<std::unique_ptr<Character>>& targets,
                        BattleContext& context, int& selectedIndex);
Return_Flags selectPlayer(std::vector<Player*>& allies, int& selectedIndex);
Return_Flags menuPlayer(Game& game);
Return_Flags chooseTarget(BattleContext& context, TargetingComponent& targetingComponent);

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