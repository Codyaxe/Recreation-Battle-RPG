#ifndef SPELLSEFFECT_H
#define SPELLSEFFECT_H
#include <string>
#include <vector>
#include <iostream>
class Character;


std::vector<int> chooseTarget(std::vector<Character>& targetInv, std::string& name, int numberofTargets, bool targetPlayer);
void applyDamage(Character& target, int damageType);
void applyHeal(Character& target);
void applyBuff(Character& target);
void applyDebuff(Character& target);


#endif