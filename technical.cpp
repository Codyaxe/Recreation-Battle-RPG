#include "technical.h"
#include "character.h"
#include "action.h"
#include "attacks.h"
#include "buffs.h"
#include "debuffs.h"
#include "items.h"
#include "spells.h"
#include "spelleffects.h"
#include "summons.h"
#include "traits.h"

HANDLE Interface::hOut;
HANDLE Interface::hIn;


void clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

Game::Game(){
    for(int i = 0; i < 6; i++){
        enemyField.push_back(Enemy());
    }
    for(int i = 0; i < 6; i++){
        playerField.push_back(Player());
    }
}

Game::~Game(){

}

void Game::attack(Player& player){

};

void Game::defend(Player& player){

};

void Game::spell(Player& player){
    Action* spell = menuChoose(player, SPELL);
    if (spell == nullptr){
        return;
    }
    (*spell)(enemyField, player);
};

void Game::item(Player& player){

};

void Game::summon(Player& player){

};


void displayMenu(int selectedIndex, bool resetDisplay) {
    if (resetDisplay == true){
        clearScreen();
    }
    std::cout << "Choose your action:\n\n";
    for (int i = 0; i < NUM_ACTIONS; ++i) {
        if (i == selectedIndex) {
            std::cout << " > " << HIGHLIGHT << actions[i] << RESET << '\n';
        } else {
            std::cout << "   " << actions[i] << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}

template <typename T>
void displayChooseMenu(int selectedIndex, std::vector<T*>& inv, const int& type) {

    switch(type){
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
    }
    
    for (int i = 0; i < inv.size(); ++i) {
        if (i == selectedIndex) {
            std::cout << " > " << HIGHLIGHT << inv[i]->name << RESET << '\n';
        } else {
            std::cout << "   " << inv[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}

void displayTargetMenu(int iter, int selectedIndex, std::vector<Enemy*>& targetInv, const std::string& name, const int& type) {

    switch(type){
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
    }
    
    for (int i = 0; i < targetInv.size(); ++i) {
        if (i == selectedIndex) {
            std::cout << " > " << HIGHLIGHT << targetInv[i]->name << RESET << '\n';
        } else {
            std::cout << "   " << targetInv[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}

void displayPlayerSelect(int selectedIndex, std::vector<Player*>& playerInv) {
    clearScreen();
    for (int i = 0; i < playerInv.size(); ++i) {
        if (i == selectedIndex) {
            std::cout << " > " << HIGHLIGHT << playerInv[i]->name << RESET << '\n';
        } else {
            std::cout << "   " << playerInv[i]->name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}



int menu(Game& game, Player& player){
    int count = 0;
    displayMenu(count);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                
                switch (vkCode) {
                    case VK_ESCAPE:
                        clearScreen();
                        std::cout << "Game menu exited.\n";
                        return EXIT;
                        
                    case VK_UP:
                        count = (count - 1 + NUM_ACTIONS) % NUM_ACTIONS;
                        displayMenu(count);
                        break;
                        
                    case VK_DOWN:
                        count = (count + 1) % NUM_ACTIONS;
                        displayMenu(count);
                        break;
                        
                    case VK_RETURN:
                        clearScreen();
                        std::cout << "Executing: " << actions[count] << "\n";
                        
                        switch (count) {
                            case 0:
                                std::cout << "Attack selected!\n";
                                game.attack(player);
                                Sleep(1000);
                                break;
                            case 1: 
                                std::cout << "Defend selected!\n";
                                game.defend(player);
                                Sleep(1000);
                                break;
                            case 2: 
                                std::cout << "Spell selected!\n";
                                game.spell(player);
                                Sleep(1000);
                                break;
                            case 3: 
                                std::cout << "Item selected!\n";
                                game.item(player);
                                Sleep(1000);
                                break;
                            case 4: 
                                std::cout << "Summon selected!\n";
                                game.summon(player);
                                Sleep(1000);
                                break;
                            case 5: 
                                std::cout << "Skip selected!\n";
                                Sleep(1000);
                                return SKIP;
                        }
                        displayMenu(count); // Redisplay menu after action
                        break;
                        
                    default:
                        // Handle other keys if needed
                        if (asciiChar >= 32 && asciiChar <= 126) {
                            // Printable character - could be used for shortcuts
                            // For now, ignore this shit
                        }
                        break;
                }
            }
        }
    }
}

Action* menuChoose(Player& player, const int& type){
    switch(type){
    case SPELL:{
        auto& inv = player.spellInv;
        return menuChooseHelper(inv, SPELL);
    }
    case ATTACK:{
        auto& inv = player.attackInv;
        return menuChooseHelper(inv, ATTACK);
    }
    case SUMMON:{
        auto& inv = player.summonInv;
        return menuChooseHelper(inv, SUMMON);
    }
    case ITEM:{
        auto& inv = player.itemInv;
        return menuChooseHelper(inv, ITEM);
    }
    case DEBUFF:{
        auto& inv = player.debuffInv;
        return menuChooseHelper(inv, DEBUFF);
    }
    case BUFF:{
        auto& inv = player.buffInv;
        return menuChooseHelper(inv, BUFF);
    }
    case TRAIT:{
        auto& inv = player.traitInv;
        return menuChooseHelper(inv, TRAIT);
    }
    }
    return nullptr;
}

template <typename T>
T* menuChooseHelper(std::vector<T*>& inv,const int& type){
    int count = 0;
    displayChooseMenu(count, inv, type);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                
                switch (vkCode) {
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

int menuTarget(int iter, std::vector<Enemy*>& targetInv, const std::string& name, const int& type){
    int count = 0;
    displayTargetMenu(iter, count, targetInv, name, type);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                
                switch (vkCode) {
                    case VK_ESCAPE:
                        clearScreen();
                        std::cout << "Target Menu exited.\n";
                        return EXIT;
                        
                    case VK_UP:
                        count = (count - 1 + targetInv.size()) % targetInv.size();
                        displayTargetMenu(iter, count, targetInv, name, type);
                        break;
                        
                    case VK_DOWN:
                        count = (count + 1) % targetInv.size();
                        displayTargetMenu(iter, count, targetInv, name, type);
                        break;
                        
                    case VK_RETURN:
                        clearScreen();
                        std::cout << "Targeting: " << targetInv[count]->name << "\n";
                        return count;
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

int selectPlayer(std::vector<Player*>& playerInv){

    int count = 0;
    displayPlayerSelect(count, playerInv);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(Interface::hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                WORD vkCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                CHAR asciiChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                
                switch (vkCode) {
                    case VK_ESCAPE:
                        clearScreen();
                        std::cout << "Target Menu exited.\n";
                        return EXIT;
                        
                    case VK_UP:
                        count = (count - 1 + playerInv.size()) % playerInv.size();
                        displayPlayerSelect(count, playerInv);
                        break;
                        
                    case VK_DOWN:
                        count = (count + 1) % playerInv.size();
                        displayPlayerSelect(count, playerInv);
                        break;
                        
                    case VK_RETURN:
                        clearScreen();
                        std::cout << "Selecting: " << playerInv[count]->name << "\n";
                        return count;
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

void menuPlayer(Game& game){
    std::vector<Player*> tracker;
    for (Player& player : game.playerField) {
        tracker.push_back(&player);
    }
    while (!tracker.empty()){
        int selectedIndex;
        do{
        selectedIndex = selectPlayer(tracker);
        if (selectedIndex == EXIT) {
            std::cout << "You must choose before you can exit!" << '\n';
        } }while(selectedIndex == EXIT);

        Player& selectedPlayer = *tracker[selectedIndex];
        menu(game, selectedPlayer);
        tracker.erase(tracker.begin() + selectedIndex);
    }
}

std::vector<int> chooseTarget(std::vector<Enemy>& targetInv, std::string& name, int numberofTargets, bool targetPlayer){
    std::vector<Enemy*> tracker;
    for(auto& enemy : targetInv){
        tracker.push_back(&enemy);
    }

    std::vector<int> targets;
    int i = 0;
    int selectedIndex;

    while(i < numberofTargets){
        selectedIndex = menuTarget(i, tracker, name, SPELL);
        //Placeholder, this will act as an end turn;
        if(selectedIndex == EXIT){
            return targets;
        }
        tracker.erase(tracker.begin() + selectedIndex);
        targets.push_back(selectedIndex);
        i++;
    }

    return targets;
};



bool Interface::enableFlags() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    
    
    if (!GetConsoleMode(hOut, &originalOutMode)) {
        std::cerr << "Failed to get console output mode.\n";
        return false;
    }

    if (!GetConsoleMode(hIn, &originalInMode)) {
        std::cerr << "Failed to get console input mode.\n";
        return false;
    }

    DWORD outMode = originalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    
    DWORD inMode = originalInMode | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

    if (!SetConsoleMode(hOut, outMode)) {
        std::cerr << "Failed to set console output mode.\n";
        return false;
    }

    if (!SetConsoleMode(hIn, inMode)) {
        std::cerr << "Failed to set console input mode.\n";
        return false;
    }
    
    return true;
}

void Interface::restoreConsoleMode() {
    SetConsoleMode(hOut, originalOutMode);
    SetConsoleMode(hIn, originalInMode);
}

Interface::Interface(){
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) {
        std::cerr << "Invalid console handles.\n";
        std::exit(1);
    }
    
    if (!enableFlags()) {
        std::cerr << "Failed to enable console flags.\n";
        std::exit(1);
    }
}

Interface::~Interface() {
    restoreConsoleMode();
}

void Interface::start(){
    SetConsoleTitleW(L"Battle RPG");
    Game game;
    menuPlayer(game);

}

    





