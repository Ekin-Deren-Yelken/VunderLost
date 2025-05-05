#pragma once
#include <string>
#include <vector>
#include "Effect.h"

namespace core {

struct DiceSpec { int count, sides; int bonus; };

struct Ability {
    std::string         id;                    // refernce id
    std::string         name;                  // Ability name
    std::string         profession;            // Can profession use an ability
    int                 cost         = 0;      // mana or health cost

    DamageType          dType;
    DiceSpec            damageRoll;            // Which Dice to use
    DiceSpec            hitRoll;               // use bonus as the “to‐hit” target number
    int                 hitThreshold;          // Threshold for ability to hit

    std::vector<Effect> effects;               // damage + status effects

    // Metadata for hit‐chance, positioning, QTE:
    int                 range        = 1;      // grid distance
    bool                requiresQTE  = false;  // parry timing?
    int                 qteWindowMs  = 0;      // timing for extra effect...
    std::string         comment;

    // Check if you can use
    bool canUse(int currentMana) const {
        return currentMana >= cost;
    }
};

} // namespace core
