#include "spelleffects.h"
#include "technical.h"

std::vector<int> chooseTarget(std::vector<Character>& targetInv, std::string& name, int numberofTargets, bool targetPlayer){
    std::vector<int> targets;
    int i = 0;
    int chosen;

    while(i < targets.size()){
        chosen = helperMenuTarget(i, targetInv, name, SPELL);
        if(chosen == EXIT){
            return;
        }
        targets.push_back(chosen);
        i++;
    }


};



void applyDamage(Character& target, int damageType){

};

void applyHeal(Character& target){
    
};

void applyBuff(Character& target){

};

void applyDebuff(Character& target){

};