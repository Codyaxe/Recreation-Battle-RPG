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
#include <thread>

HANDLE Interface::hOut;
HANDLE Interface::hIn;

void clearScreen()
{
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

// Waits for the user to press enter
bool processSkip(std::mutex& skipMutex, std::condition_variable& skipCv, bool& skipPressed,
                 bool& shouldExit)
{

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true)
    {
        // For cases that requires the thread to end immediately without pressing enter
        {
            std::lock_guard<std::mutex> lock(skipMutex);
            if (shouldExit)
                break;
        }

        DWORD waitResult = WaitForSingleObject(Interface::hIn, 50);

        if (waitResult == WAIT_OBJECT_0)
        {
            if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0)
            {
                if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown &&
                    inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                {
                    {
                        std::lock_guard<std::mutex> lock(skipMutex);
                        skipPressed = true;
                    }
                    skipCv.notify_one();
                    break;
                }
            }
        }
    }
    return true;
}

Game::Game()
{
    // To instantiate player and enemy objects including their inventories
    for (int i = 0; i < 6; i++)
    {
        auto enemy = std::make_unique<Enemy>("Generic " + std::to_string(i));
        enemies.push_back(std::move(enemy));
    }
    for (int i = 0; i < 6; i++)
    {
        auto ally = std::make_unique<Player>("Generic " + std::to_string(i));
        allies.push_back(std::move(ally));
    }
    for (int i = 0; i < 6; i++)
    {
        auto spell = std::make_unique<Fireball>();
        auto spell_2 = std::make_unique<Poison_Gas>();
        allies[i]->spellInv.push_back(std::move(spell));
        allies[i]->spellInv.push_back(std::move(spell_2));
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

void displayChooseMenu(int selectedIndex, std::vector<std::unique_ptr<Action>>& inv,
                       const ActionType& type)
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
                       BattleContext& context)
{
    clearScreen();

    switch (context.actionType)
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
    std::cout << "Choose a player:" << "\n\n";
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

Return_Flags menu(Game& game, Player& player)
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
                    return Return_Flags::EXIT;

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
                    Action_Result result = player.commands[count].execute(game);
                    switch (result)
                    {
                    case Action_Result::END_TURN:
                        return Return_Flags::END;
                    case Action_Result::SKIP_TURN:
                        return Return_Flags::SKIP;
                    case Action_Result::CONTINUE_TURN:
                        break;
                    case Action_Result::END_BATTLE_TURN:
                        clearScreen();
                        return Return_Flags::END_BATTLE;
                    case Action_Result::EXTRA_TURN:
                        break;
                    case Action_Result::INTERRUPT_TURN:
                        break;
                    case Action_Result::RESULT_NONE:
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

Action* menuChooseHelper(std::vector<std::unique_ptr<Action>>& inv, const ActionType& type)
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
                    return inv[count].get();
                    break;

                default:
                    break;
                }
            }
        }
    }
}

Return_Flags menuTarget(int iter, std::vector<Character*>& targets, BattleContext& context,
                        int& selectedIndex)
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
                    return Return_Flags::EXIT;

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
                    selectedIndex = count;
                    return Return_Flags::SUCCESS;

                default:
                    break;
                }
            }
        }
    }
}

Return_Flags selectPlayer(std::vector<Player*>& allies, int& selectedIndex)
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
                    std::cout << "You've quited the game" << '\n';
                    // Keyboard Listening if Player wants to exit, cancel, or just end the turn
                    Sleep(1000);
                    return Return_Flags::QUIT;

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
                    selectedIndex = count;
                    return Return_Flags::SUCCESS;

                default:
                    break;
                }
            }
        }
    }
}

Return_Flags menuPlayer(Game& game)
{
    std::vector<Player*> tracker;
    for (auto& player : game.allies)
    {
        tracker.push_back(dynamic_cast<Player*>(player.get()));
    }
    while (!tracker.empty())
    {
        int selectedIndex;
        Return_Flags selectResult;
        do
        {
            Return_Flags selectResult = selectPlayer(tracker, selectedIndex);
        } while (selectResult == Return_Flags::EXIT);

        if (selectResult == Return_Flags::END_BATTLE)
        {
            return Return_Flags::END_BATTLE;
        }
        if (selectResult == Return_Flags::QUIT)
        {
            return Return_Flags::QUIT;
        }

        Player& selectedPlayer = *tracker[selectedIndex];
        Return_Flags menuResult = menu(game, selectedPlayer);
        if (menuResult == Return_Flags::EXIT)
        {
            continue;
        }
        if (menuResult == Return_Flags::END_BATTLE)
        {
            return Return_Flags::END_BATTLE;
        }
        tracker.erase(tracker.begin() + selectedIndex);
    }
    return Return_Flags::SUCCESS;
}

Return_Flags chooseTarget(BattleContext& context, TargetingComponent& targetingComponent)
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
        std::transform(context.enemies.begin(), context.enemies.end(),
                       std::back_inserter(potentialTargets),
                       [](const std::unique_ptr<Character>& c) { return c.get(); });
    }
    if (targetingComponent.faction == TargetFaction::ALLIES ||
        targetingComponent.faction == TargetFaction::BOTH)
    {
        std::transform(context.allies.begin(), context.allies.end(),
                       std::back_inserter(potentialTargets),
                       [](const std::unique_ptr<Character>& c) { return c.get(); });
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
            Return_Flags resultTarget = menuTarget(i, validTargets, context, selectedIndex);
            if (resultTarget == Return_Flags::EXIT)
            {
                return Return_Flags::EXIT;
            }
            context.currentTargets.push_back(validTargets[selectedIndex]);
            validTargets.erase(validTargets.begin() + selectedIndex);
            i++;
        }
        return Return_Flags::SUCCESS;
    }
    return Return_Flags::SUCCESS;
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
    static EventObserver eventBattleContext;
    std::thread listener(&EventObserver::trigger, &eventBattleContext, std::ref(game));

    while (true)
    {
        Return_Flags resultGame = menuPlayer(game);
        if (resultGame == Return_Flags::END_BATTLE)
        {
            eventBattleContext.enqueue(EventCondition::ON_END_TURN);
            eventBattleContext.waitForEventProcessing();
        }
        if (resultGame == Return_Flags::QUIT)
        {
            break;
        }
    }

    // While loop for end turn and enemy turn

    // For now quit!
    eventBattleContext.setQuit();
    listener.join();
}
