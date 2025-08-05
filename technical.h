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

enum TargetType{
    SPELL = 10,
    ATTACK = 11,
    SUMMON = 12,
    ITEM = 13
};

enum Exit_Flag{
    EXIT = -1,
};

void clearScreen();

void displayMenu(int selectedIndex, bool resetDisplay = true);
void displayTargetMenu(int selectedIndex, std::vector<Character>& targetInv, const std::string& name, const int& type);
void menu();
int helperMenuTarget(int iter, std::vector<Character>& targetInv, const std::string& name, const int& type);
int menuTarget(const Interface& interface_, std::vector<Character>& targetInv, const std::string& name, const int& type);
std::vector<int> chooseTarget(std::vector<Character>& targetInv, std::string& name, int numberofTargets, bool targetPlayer = false);

class Game;
class Interface;

#endif