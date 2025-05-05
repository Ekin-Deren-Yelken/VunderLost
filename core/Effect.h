#pragma once
#include <string>

    // Define any attack or spell effect in the game

namespace core {
    enum class DamageType { 
        Physical,    // Medium Damage, High Accuracy
        Fire,        // High Damage, Low Accuracy
        Lightning,   // Mega Damage, Low Accuracy
        Frost,       // Medium Damage, Medium Accuracy
        Poison       // Low Damage, Mega Accuracy
    };
    enum class StatusType {
        None,        // no status
        Bleed,       // damage over time
        Scorched,    // fire-based DoT
        Shocked,     // accuracy penalty by value*25%
        Frostbitten, // slows or other frost DoT
        Poisoned,    // poison DoT
        Fortify,     // NEW reduce next damage taken by value *10%
        DMG          // NEW take incerased damage by value*10%
    }; 

    /**
     * An Effect is either:
     *  - Instant damage of some DamageType, or
     *  - Status/debuff (with optional damage-over-time)
     *  - Both in the same struct
     */

    // Simplify pipeline by unifying these two things into one struct
    // Allows ability to add both isntant damage and status effects at the same time

    struct Effect {
        // Single field per effect
        int        dmgValue; // amount of instant damage
        int        dotValue;
        StatusType status;   // for Damage over time (DoT) or debuff effects
        int        duration;  // turns remaining for debuff or DoT
        core::DamageType dmgType = core::DamageType::Physical;

        std::string source; 

        // Constructor for Damage. Easy to access everything.
        Effect(DamageType dt, int val)
            : dmgType(dt)
            , dmgValue(val)
            , status(StatusType::None)
            , duration(0)
        {}

        Effect(StatusType s, int dur, int dot = 0, int dmg = 0, DamageType type = DamageType::Physical)
            : status(s)
            , duration(dur)
            , dotValue(dot)
            , dmgValue(dmg)
            , dmgType(type)
        {}
    };

}