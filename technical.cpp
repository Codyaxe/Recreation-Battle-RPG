#include "technical.h"
#include "action.h"
#include "attacks.h"
#include "buffs.h"
#include "character.h"
#include "debuffs.h"
#include "items.h"
#include "spelleffects.h"
#include "spells.h"
#include "summons.h"
#include "traits.h"

HANDLE Interface::hOut;
HANDLE Interface::hIn;

void clearScreen()
{
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

Game::Game()
{
    for (int i = 0; i < 6; i++)
    {
        enemies.push_back(new Enemy());
    }
    for (int i = 0; i < 6; i++)
    {
        allies.push_back(new Player("Generic " + std::to_string(i)));
    }
    for (int i = 0; i < 6; i++)
    {
        allies[i]->spellInv.push_back(new Fireball());
    }
}

Game::~Game() {}

void displayMenu(int selectedIndex, Player& player, bool resetDisplay)
{
    if (resetDisplay == true)
    {
        clearScreen();
    }
    std::cout << "Choose your action:\n\n";
    for (int i = 0; i < player.commandsSize; ++i)
    {
        if (i == selectedIndex)
        {
            std::cout << " > " << HIGHLIGHT << player.commands[i].name << RESET << '\n';
        }
        else
        {
            std::cout << "   " << player.commands[i].name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush();
}

void displayChooseMenu(int selectedIndex, std::vector<Action*>& inv, const int& type)
{

    switch (type)
    {
    case SPELL:
        std::cout << "Choose a spell: \n";
        break;

    case ATTACK:
        std::cout << "Choose an attack: \n";
        break;

    case SUMMON:
        std::cout << "Choose a summon: \n";
        break;

    case ITEM:
        std::cout << "Choose an item: \n";
        break;

    default:
        std::cout << "Choose a/an [REDACTED]: \n";
        break;
    }

    for (int i = 0; i < inv.size(); ++i)
    {
        if (i == selectedIndex)
        {
            std::cout << " > " << HIGHLIGHT << inv[i]->name << RESET << '\n';
        }
        else
        {
            std::cout << "   " << inv[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush();
}

void displayTargetMenu(int iter, int selectedIndex, std::vector<Character*>& targets,
                       const std::string& name, const int& type)
{
    clearScreen();

    switch (type)
    {
    case SPELL:
        std::cout << "Choose target" << iter + 1 << " to cast " << name << " on: \n";
        break;

    case ATTACK:
        std::cout << "Choose target" << iter + 1 << " to initiate " << name << " on: \n";
        break;

    case SUMMON:
        std::cout << "Choose target" << iter + 1 << " to summon " << name << " on: \n";
        break;

    case ITEM:
        std::cout << "Choose target" << iter + 1 << " to use " << name << " on: \n";
        break;

    default:
        std::cout << "Choose target" << iter + 1 << " to [REDACTED] " << name << " on: \n";
        ;
        break;
    }

    for (int i = 0; i < targets.size(); ++i)
    {
        if (i == selectedIndex)
        {
            std::cout << " > " << HIGHLIGHT << targets[i]->name << RESET << '\n';
        }
        else
        {
            std::cout << "   " << targets[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush();
}

void displayPlayerSelect(int selectedIndex, std::vector<Player*>& allies)
{
    clearScreen();
    for (int i = 0; i < allies.size(); ++i)
    {
        if (i == selectedIndex)
        {
            std::cout << " > " << HIGHLIGHT << allies[i]->name << RESET << '\n';
        }
        else
        {
            std::cout << "   " << allies[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush();
}

int menu(Game& game, Player& player)
{
    int count = 0;
    displayMenu(count, player);

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true)
    {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
        {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
            {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                switch (vkCode)
                {
                case VK_ESCAPE:
                    clearScreen();
                    std::cout << "Action selection exited.\n";
                    Sleep(1000);
                    return EXIT;

                case VK_UP:
                    count = (count - 1 + player.commandsSize) % player.commandsSize;
                    displayMenu(count, player);
                    break;

                case VK_DOWN:
                    count = (count + 1) % player.commandsSize;
                    displayMenu(count, player);
                    break;

                case VK_RETURN:
                {
                    clearScreen();
                    std::cout << "Executing: " << player.commands[count].name << "\n";
                    std::cout << player.commands[count].name << " selected!\n";
                    int result = player.commands[count].execute(game);
                    switch (result)
                    {
                    case END_TURN:
                        return END;

                    case SKIP_TURN:
                        return SKIP;

                    case CONTINUE_TURN:
                        break;

                    case EXTRA_TURN:
                        break;

                    case INTERRUPT_TURN:
                        return END;

                    case RESULT_NONE:
                        break;

                    default:
                        break;
                    }
                    Sleep(1000);
                    displayMenu(count, player); // Redisplay menu after action
                    break;
                }
                default:
                    // Handle other keys if needed
                    if (asciiChar >= 32 && asciiChar <= 126)
                    {
                        // Printable character - could be used for shortcuts
                        // For now, ignore this shit
                    }
                    break;
                }
            }
        }
    }
}

Action* menuChoose(Character& player, const int& type)
{
    switch (type)
    {
    case SPELL:
    {
        auto& inv = player.spellInv;
        return menuChooseHelper(inv, SPELL);
    }

    case ATTACK:
    {
        auto& inv = player.attackInv;
        return menuChooseHelper(inv, ATTACK);
    }

    case SUMMON:
    {
        auto& inv = player.summonInv;
        return menuChooseHelper(inv, SUMMON);
    }

    case ITEM:
    {
        auto& inv = player.itemInv;
        return menuChooseHelper(inv, ITEM);
    }

    case DEBUFF:
    {
        auto& inv = player.debuffInv;
        return menuChooseHelper(inv, DEBUFF);
    }

    case BUFF:
    {
        auto& inv = player.buffInv;
        return menuChooseHelper(inv, BUFF);
    }

    case TRAIT:
    {
        auto& inv = player.traitInv;
        return menuChooseHelper(inv, TRAIT);
    }

    default:
        break;
    }
    return nullptr;
}

Action* menuChooseHelper(std::vector<Action*>& inv, const int& type)
{
    int count = 0;
    displayChooseMenu(count, inv, type);

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true)
    {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
        {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
            {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                switch (vkCode)
                {
                case VK_ESCAPE:
                    clearScreen();
                    std::cout << "Target Menu exited.\n";
                    return nullptr;

                case VK_UP:
                    count = (count - 1 + inv.size()) % inv.size();
                    displayChooseMenu(count, inv, type);
                    break;

                case VK_DOWN:
                    count = (count + 1) % inv.size();
                    displayChooseMenu(count, inv, type);
                    break;

                case VK_RETURN:
                    clearScreen();
                    std::cout << "Choosing: " << inv[count]->name << "\n";
                    return inv[count];
                    break;

                default:
                    break;
                }
            }
        }
    }
}

int menuTarget(int iter, std::vector<Character*>& targets, const std::string& name, const int& type)
{
    int count = 0;
    displayTargetMenu(iter, count, targets, name, type);

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true)
    {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
        {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
            {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                switch (vkCode)
                {
                case VK_ESCAPE:
                    clearScreen();
                    std::cout << "Target Menu exited.\n";
                    return EXIT;

                case VK_UP:
                    count = (count - 1 + targets.size()) % targets.size();
                    displayTargetMenu(iter, count, targets, name, type);
                    break;

                case VK_DOWN:
                    count = (count + 1) % targets.size();
                    displayTargetMenu(iter, count, targets, name, type);
                    break;

                case VK_RETURN:
                    clearScreen();
                    std::cout << "Targeting: " << targets[count]->name << "\n";
                    return count;
                    break;

                default:
                    break;
                }
            }
        }
    }
}

int selectPlayer(std::vector<Player*>& allies)
{

    int count = 0;
    displayPlayerSelect(count, allies);

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true)
    {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
        {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
            {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                switch (vkCode)
                {
                case VK_ESCAPE:
                    clearScreen();
                    std::cout << "You must choose before you can exit!\n";
                    Sleep(1000);
                    return EXIT;

                case VK_UP:
                    count = (count - 1 + allies.size()) % allies.size();
                    displayPlayerSelect(count, allies);
                    break;

                case VK_DOWN:
                    count = (count + 1) % allies.size();
                    displayPlayerSelect(count, allies);
                    break;

                case VK_RETURN:
                    clearScreen();
                    std::cout << "Selecting: " << allies[count]->name << "\n";
                    Sleep(1000);
                    return count;
                    break;

                default:
                    break;
                }
            }
        }
    }
}

void menuPlayer(Game& game)
{
    std::vector<Player*> tracker;
    for (auto* player : game.allies)
    {
        tracker.push_back(dynamic_cast<Player*>(player));
    }
    while (!tracker.empty())
    {
        int selectedIndex;
        do
        {
            selectedIndex = selectPlayer(tracker);
        } while (selectedIndex == EXIT);

        Player& selectedPlayer = *tracker[selectedIndex];
        int menuState = menu(game, selectedPlayer);
        if (menuState == EXIT)
        {
            continue;
        }
        tracker.erase(tracker.begin() + selectedIndex);
    }
}

std::vector<int> chooseTarget(std::vector<Character*>& enemies, std::vector<Character*>& allies,
                              std::string& name, int numberofTargets, int groupTarget, int type)
{
    std::vector<Character*> candidates;

    switch (groupTarget)
    {
    case ENEMIES:
        for (auto& enemy : enemies)
        {
            candidates.push_back(enemy);
        }
        break;
    case ALLIES:
        for (auto& ally : allies)
        {
            candidates.push_back(ally);
        }
        break;
    case BOTH:
        for (auto& enemy : enemies)
        {
            candidates.push_back(enemy);
        }
        for (auto& ally : allies)
        {
            candidates.push_back(ally);
        }
        break;
    }

    std::vector<int> targets;
    int i = 0;
    int selectedIndex;

    while (i < numberofTargets)
    {
        selectedIndex = menuTarget(i, candidates, name, type);
        if (selectedIndex == EXIT)
        {
            return targets;
        }
        candidates.erase(candidates.begin() + selectedIndex);
        targets.push_back(selectedIndex);
        i++;
    }

    return targets;
};

bool Interface::enableFlags()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn = GetStdHandle(STD_INPUT_HANDLE);

    if (!GetConsoleMode(hOut, &originalOutMode))
    {
        std::cerr << "Failed to get console output mode.\n";
        return false;
    }

    if (!GetConsoleMode(hIn, &originalInMode))
    {
        std::cerr << "Failed to get console input mode.\n";
        return false;
    }

    DWORD outMode = originalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    DWORD inMode = originalInMode | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

    if (!SetConsoleMode(hOut, outMode))
    {
        std::cerr << "Failed to set console output mode.\n";
        return false;
    }

    if (!SetConsoleMode(hIn, inMode))
    {
        std::cerr << "Failed to set console input mode.\n";
        return false;
    }

    return true;
}

void Interface::restoreConsoleMode()
{
    SetConsoleMode(hOut, originalOutMode);
    SetConsoleMode(hIn, originalInMode);
}

Interface::Interface()
{
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Invalid console handles.\n";
        std::exit(1);
    }

    if (!enableFlags())
    {
        std::cerr << "Failed to enable console flags.\n";
        std::exit(1);
    }
}

Interface::~Interface() { restoreConsoleMode(); }

void Interface::start()
{
    SetConsoleTitleW(L"Battle RPG");
    Game game;
    menuPlayer(game);
}
