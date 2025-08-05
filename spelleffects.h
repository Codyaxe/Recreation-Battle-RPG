#ifndef SPELLSEFFECT_H
#define SPELLSEFFECT_H
#include <string>
#include <vector>
#include <iostream>
class Character;

enum SpellType{
    PHYSICAL = 30,
    FIRE = 31,
    WATER = 32,
    WIND = 33,
    NATURE = 34,
    POISON = 35,
    ELECTRIC = 36,
    GROUND  = 37,
    PSYCHIC = 38,
    ROCK = 39,
    ICE = 40,
    BIO = 41,
    CHEM = 42,
    GEOM = 43,
    MUTATE = 44,
    PARANORMAL = 45,
    DARK = 46,
    STEEL = 47,
    CHAOS = 48,
    ORDER = 49,
    MYSTIC = 50,
    HOLY = 51,
    PLASMA = 52,
    BOMB = 53,
    BLOOD = 54,
    SHARD = 55,
    CRYSTAL = 56,
    LIGHT = 57,
    BREAKDOWN = 58,
    SPECIAL = 59,
    WEAPON = 60,
    INTERDIMENSIONAL = 61,
    SOUND = 62,
    AETHER = 63,
    FORCE = 64,
    LOGOS = 65,
    ETHOS = 66,
    PATHOS = 67,
    ALMIGHTY = 68,
    RUNE = 69,
    DREAM = 70,
    MAGMA = 71,
    TECH = 72,
    RADIOACTIVE = 73,
    CHRONO = 74,
    LUCK = 75,
    FATE = 76
};

void applyDamage(Character& target, int damage, int damageType);
void applyHeal(Character& target);
void applyBuff(Character& target);
void applyDebuff(Character& target);


#endif