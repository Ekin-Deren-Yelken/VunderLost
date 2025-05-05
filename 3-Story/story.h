#ifndef STORY_H
#define STORY_H

#include "../1-Characters/character.h"

// Runs the full intro story sequence
void runIntroScene();

// Internal part of intro: loop until player chooses "mirror"
void loopUntilMirrorChoice(Character& player);

void runCharacterSelection(Character& player);


#endif
