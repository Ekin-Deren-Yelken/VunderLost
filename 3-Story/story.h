#ifndef STORY_H
#define STORY_H

#include "../1-Characters/character.h"

// Runs the full intro story sequence
void runIntroScene();
std::string getInput(const std::string& prompt, bool toLower = false);

// Internal part of intro: loop until player chooses "mirror"
void loopUntilMirrorChoice(Character& player);

void runCharacterSelection(Character& player);

void runProfessionSelection(Character& player);


#endif
