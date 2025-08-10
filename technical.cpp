#include "technical.h"
#include "action.h"
#include "attacks.h"
#include "buffs.h"
#include "character.h"
#include "debuffs.h"
#include "effects.h"
#include "items.h"
#include "observer.h"
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

void displayChooseMenu(int selectedIndex, std::vector<Action*>& inv, const ActionType& type)
{
    clearScreen();
    switch (type)
    {
    case ActionType::SPELL:
        std::cout << "Choose a spell: \n";
        break;

    case ActionType::ATTACK:
        std::cout << "Choose an attack: \n";
        break;

    case ActionType::SUMMON:
        std::cout << "Choose a summon: \n";
        break;

    case ActionType::ITEM:
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
                       Observer& context)
{
    clearScreen();

    switch (context.type)
    {
    case ActionType::SPELL:
        std::cout << "Choose target " << iter + 1 << " to cast " << context.name << " on: \n";
        break;

    case ActionType::ATTACK:
        std::cout << "Choose target " << iter + 1 << " to initiate " << context.name << " on: \n";
        break;

    case ActionType::SUMMON:
        std::cout << "Choose target " << iter + 1 << " to summon " << context.name << " on: \n";
        break;

    case ActionType::ITEM:
        std::cout << "Choose target " << iter + 1 << " to use " << context.name << " on: \n";
        break;

    default:
        std::cout << "Choose target " << iter + 1 << " to [REDACTED] " << context.name << " on: \n";
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

Action* menuChoose(Character& player, const ActionType& type)
{
    switch (type)
    {
    case ActionType::SPELL:
    {
        auto& inv = player.spellInv;
        return menuChooseHelper(inv, ActionType::SPELL);
    }

    case ActionType::ATTACK:
    {
        auto& inv = player.attackInv;
        return menuChooseHelper(inv, ActionType::ATTACK);
    }

    case ActionType::SUMMON:
    {
        auto& inv = player.summonInv;
        return menuChooseHelper(inv, ActionType::SUMMON);
    }

    case ActionType::ITEM:
    {
        auto& inv = player.itemInv;
        return menuChooseHelper(inv, ActionType::ITEM);
    }

    case ActionType::DEBUFF:
    {
        auto& inv = player.debuffInv;
        return menuChooseHelper(inv, ActionType::DEBUFF);
    }

    case ActionType::BUFF:
    {
        auto& inv = player.buffInv;
        return menuChooseHelper(inv, ActionType::BUFF);
    }

    case ActionType::TRAIT:
    {
        auto& inv = player.traitInv;
        return menuChooseHelper(inv, ActionType::TRAIT);
    }

    default:
        break;
    }
    return nullptr;
}

Action* menuChooseHelper(std::vector<Action*>& inv, const ActionType& type)
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
                    Sleep(1000);
                    return inv[count];
                    break;

                default:
                    break;
                }
            }
        }
    }
}

int menuTarget(int iter, std::vector<Character*>& targets, Observer& context)
{
    int count = 0;
    displayTargetMenu(iter, count, targets, context);

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
                    displayTargetMenu(iter, count, targets, context);
                    break;

                case VK_DOWN:
                    count = (count + 1) % targets.size();
                    displayTargetMenu(iter, count, targets, context);
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

bool chooseTarget(Observer& context, TargetingComponent& targetingComponent)
{
    // If the observer has current targets, due to spells having two targeting phases for two
    // effects, clear previous targets.
    if (!context.currentTargets.empty())
    {
        context.currentTargets.clear();
        context.damageDealt.clear();
    }
    std::vector<Character*> potentialTargets;
    std::vector<Character*> validTargets;

    if (targetingComponent.faction == TargetFaction::ENEMIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        potentialTargets.insert(potentialTargets.end(), context.enemies.begin(),
                                context.enemies.end());
    }
    if (targetingComponent.faction == TargetFaction::ALLIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        potentialTargets.insert(potentialTargets.end(), context.allies.begin(),
                                context.allies.end());
    }
    if (targetingComponent.gameConditions.empty() && targetingComponent.targetConditions.empty())
    {
        validTargets = potentialTargets;
    }
    else if (targetingComponent.targetingConditionLogic == ConditionLogic::AND)
    {
        for (const auto& target : potentialTargets)
        {
            bool allConditionsMet = true;

            for (const auto& condition : targetingComponent.gameConditions)
            {
                if (!checkTargetHasCondition(condition, target))
                {
                    allConditionsMet = false;
                    break;
                }
            }

            for (const auto& condition : targetingComponent.targetConditions)
            {
                if (!checkTargetHasCondition(condition, target))
                {
                    allConditionsMet = false;
                    break;
                }
            }

            if (allConditionsMet)
            {
                validTargets.push_back(target);
            }
        }
    }
    else
    {
        for (const auto& target : potentialTargets)
        {

            for (const auto& condition : targetingComponent.gameConditions)
            {

                if (checkTargetHasCondition(condition, target))
                {
                    validTargets.push_back(target);
                    break;
                }
            }

            for (const auto& condition : targetingComponent.targetConditions)
            {

                if (checkTargetHasCondition(condition, target))
                {
                    validTargets.push_back(target);
                    break;
                }
            }
        }
    }

    if (!validTargets.empty())
    {
        int i = 0;
        int selectedIndex;

        while (i < targetingComponent.numberOfTargets)
        {
            // Shall the user exited, return false indicating targeting is exited
            selectedIndex = menuTarget(i, validTargets, context);
            if (selectedIndex == EXIT)
            {
                return false;
            }
            context.currentTargets.push_back(validTargets[selectedIndex]);
            validTargets.erase(validTargets.begin() + selectedIndex);
            i++;
        }
        return true;
    }
    return true;
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
