#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include "../1-Characters/character.h"

bool loadGame(Character& player);
std::string saveGame(const Character& player);

#endif