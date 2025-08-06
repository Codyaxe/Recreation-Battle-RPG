#include "technical.h"
#include "character.h"

const std::string RESET = "\033[0m";
const std::string HIGHLIGHT = "\033[47;30m";
const std::string actions[5] = { "Attack", "Defend", "Spell", "Item", "Summon" };
const int NUM_ACTIONS = sizeof(actions) / sizeof(actions[0]);


void clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

Game::Game(){
    for(int i = 0; i < 6; i++){
        enemyField.push_back(Enemy());
    }
}

Game::~Game(){

}

void Game::attack(){

};

void Game::defend(){

};

void Game::spell(){
    Spell* spell = menuChoose<Spell>(player, SPELL);
    (*spell)(enemyField);
};

void Game::item(){

};

void Game::summon(){

};


void displayMenu(int selectedIndex, bool resetDisplay = true) {
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
            std::cout << " > " << HIGHLIGHT << inv[i].name << RESET << '\n';
        } else {
            std::cout << "   " << inv[i].name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}

void displayTargetMenu(int iter, int selectedIndex, std::vector<Enemy>& targetInv, const std::string& name, const int& type) {

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
            std::cout << " > " << HIGHLIGHT << targetInv[i].name << RESET << '\n';
        } else {
            std::cout << "   " << targetInv[i].name << '\n';
        }
    }

    std::cout << "\nUse UP/DOWN arrows to move, ENTER to select, ESC to quit.";
    std::cout.flush(); 
}



void menu(Game& game){
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
                        return;
                        
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
                                game.attack();
                                Sleep(1000);
                                break;
                            case 1: 
                                std::cout << "Defend selected!\n";
                                game.defend();
                                Sleep(1000);
                                break;
                            case 2: 
                                std::cout << "Spell selected!\n";
                                game.spell();
                                Sleep(1000);
                                break;
                            case 3: 
                                std::cout << "Item selected!\n";
                                game.item();
                                Sleep(1000);
                                break;
                            case 4: 
                                std::cout << "Summon selected!\n";
                                game.summon();
                                Sleep(1000);
                                break;
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

template <typename T>
T* menuChoose(Player& player,const int& type){
    switch(type){
    case SPELL:{
        auto& inv = player.spellInv;
        return menuChooseHelper(inv, SPELL);
    }
    case ATTACK:{
        auto& inv = player.attackInv;
        return menuChooseHelper(inv, SPELL);
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
                        return;
                        
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
                        std::cout << "Choosing: " << inv[count].name << "\n";
                        return inv[count];
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

int menuTarget(int iter, std::vector<Enemy>& targetInv, const std::string& name, const int& type){
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
                        std::cout << "Targeting: " << targetInv[count].name << "\n";
                        return count;
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

std::vector<int> chooseTarget(std::vector<Enemy>& targetInv, std::string& name, int numberofTargets, bool targetPlayer = false){
    std::vector<Enemy> tempTargetInv = targetInv;
    std::vector<int> targets;
    int i = 0;
    int chosen;

    while(i < numberofTargets){
        chosen = menuTarget(i, tempTargetInv, name, SPELL);
        if(chosen == EXIT){
            return;
        }
        tempTargetInv.erase(tempTargetInv.begin() + chosen);
        targets.push_back(chosen);
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
    menu(*this);
}

    





