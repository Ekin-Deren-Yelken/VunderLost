#include "stat_utils.h"
#include "../0-utils/rpg_utils.h"
#include <iostream>

std::map<std::string, int> getBaseStats() {
    return {
        { "STR",   7 },
        { "DEX",   6 },
        { "INT",   4 },
        { "LUCK",  5 },
        { "CHARM", 5 },
        { "GIRTH", 6 },
        { "ARMOR", 0 }
    };
}

std::map<std::string, int> applyStatModifiers(
    std::map<std::string, int> baseStats,
    const std::string& race,
    const std::string& sex,
    const std::string& gender,
    const std::string& profession
) {

    // --- RACE ---
    if (race == "elf") {
        baseStats["INT"] += 3;
        baseStats["DEX"] += 2;
        baseStats["STR"] -= 3;
        baseStats["LUCK"] -= 2;
        baseStats["CHARM"] += 1;
    } else if (race == "beast") {
        baseStats["INT"] -= 2;
        baseStats["DEX"] += 2;
        baseStats["STR"] += 4;
        baseStats["LUCK"] += 2;
        baseStats["CHARM"] -= 3;
    }
    // Human = no change

    // --- SEX ---
    if (sex == "male") {
        baseStats["STR"] += 1;
        baseStats["DEX"] -= 1;
        baseStats["LUCK"] += 1;
    } else if (sex == "female") {
        baseStats["DEX"] += 1;
        baseStats["INT"] += 1;
        baseStats["CHARM"] += 2;
    }

    // --- GENDER ---
    if (gender == "homosexual" && sex == "male") {
        baseStats["CHARM"] += 2;
        baseStats["STR"] -= 1;
        baseStats["INT"] += 1;
        baseStats["LUCK"] -= 1;
    } else if (gender == "homosexual" && sex == "female") {
        baseStats["CHARM"] -= 2;
        baseStats["STR"] += 1;
        baseStats["INT"] -= 1;
        baseStats["DEX"] += 1;
    } else if (gender == "bisexual" && sex == "female") {
        baseStats["LUCK"] += 1;
    } else if (gender == "bisexual" && sex == "male") {
        baseStats["CHARM"] += 2;
        baseStats["LUCK"] -= 1;
    } else if (gender == "straight") {
        baseStats["LUCK"] += 1;
        baseStats["CHARM"] -= 1;
    }

    // --- PROFESSION ---
    if (profession == "knight") {
        baseStats["STR"] += 3;
        baseStats["GIRTH"] += 2;
    } else if (profession == "mage") {
        baseStats["INT"] += 4;
        baseStats["DEX"] -= 2;
    } else if (profession == "archer") {
        baseStats["GIRTH"] -= 1;
        baseStats["DEX"] += 3;
    } else if (profession == "thief") {
        baseStats["DEX"] += 4;
        baseStats["LUCK"] += 1;
        baseStats["INT"] += 2;
        baseStats["CHARM"] += 1;
    } else if (profession == "clown") {
        baseStats["GIRTH"] += 5;
        baseStats["INT"] -= 3;
        baseStats["DEX"] -= 2;
        baseStats["CHARM"] += 2;
        baseStats["LUCK"] += 8;
    }

    return baseStats;
}