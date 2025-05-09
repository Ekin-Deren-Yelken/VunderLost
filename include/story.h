#pragma once

#include "character.h"

// Runs the full intro story sequence
void runIntroScene();
std::string getInput(const std::string& prompt, bool toLower = false);

// Internal part of intro: loop until player chooses "mirror"
void loopUntilMirrorChoice(Character& player);
void runCharacterSelection(Character& player, int dmg);
void runProfessionSelection(Character& player, int dmg);
