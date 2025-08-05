#include "technical.h"

const std::string RESET = "\033[0m";
const std::string HIGHLIGHT = "\033[47;30m";
const std::string actions[5] = { "Attack", "Defend", "Spell", "Item", "Summon" };
const int NUM_ACTIONS = sizeof(actions) / sizeof(actions[0]);

void clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

void displayTargetMenu(int iter, int selectedIndex, std::vector<Character>& targetInv, const std::string& name, const int& type) {

    switch(type){
        case SPELL:
            std::cout << "Choose target" << iter + 1 << " to cast " << name << " on: ";
        break;
        case ATTACK:
            std::cout << "Choose target" << iter + 1 << " to initiate " << name << " on: ";
        break;
        case SUMMON:
            std::cout << "Choose target" << iter + 1 << " to summon " << name << " on: ";
        break;
        case ITEM:
            std::cout << "Choose target" << iter + 1 << " to use " << name << " on: ";
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

void menu(const Interface& interface_){
    int count = 0;
    displayMenu(count);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(interface_.hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
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
                                Sleep(1000);
                                break;
                            case 1: 
                                std::cout << "Defend selected!\n";
                                Sleep(1000);
                                break;
                            case 2: 
                                std::cout << "Spell selected!\n";
                                Sleep(1000);
                                break;
                            case 3: 
                                std::cout << "Item selected!\n";
                                Sleep(1000);
                                break;
                            case 4: 
                                std::cout << "Summon selected!\n";
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

int helperMenuTarget(int iter, std::vector<Character>& targetInv, const std::string& name, const int& type){
    return menuTarget(iter, targetInv, name, type);
}

int menuTarget(const Interface& interface_, int iter, std::vector<Character>& targetInv, const std::string& name, const int& type){
    int count = 0;
    displayTargetMenu(iter, count, targetInv, name, type);
    
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    
    while (true) {
        if (ReadConsoleInput(interface_.hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
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

class Interface{

    public:

    HANDLE hOut;
    HANDLE hIn;
    DWORD originalOutMode, originalInMode; 

    bool enableFlags() {
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

    void restoreConsoleMode() {
        SetConsoleMode(hOut, originalOutMode);
        SetConsoleMode(hIn, originalInMode);
    }

    public:

    Interface(){
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

    ~Interface() {
        restoreConsoleMode();
    }

    void start(){
        SetConsoleTitleW(L"Battle RPG");
        menu(*this);
    }

    friend void menu(const Interface& interface_);



};