#include "stat_utils.h"
#include "../0-utils/rpg_utils.h"
#include <iostream>

std::map<std::string, int> getBaseStats() {
    return {
        {"STR", 7},
        {"DEX", 7},
        {"INT", 7},
        {"GIRTH", 5},
        {"LUCK", 5},
        {"CHARM", 5}
    };
}

void applyStatModifiers(std::map<std::string, int>& stats,
                        const std::string& race,
                        const std::string& sex,
                        const std::string& gender,
                        const std::string& profession) {
    
    stats = getBaseStats();

    // Race
    if (race == "Elf") {
        stats["INT"] += 3;
        stats["DEX"] += 2;
        stats["STR"] -= 3;
        stats["LUCK"] -= 2;
        stats["CHARM"] += 1;
    } else if (race == "Beast") {
        stats["INT"] -= 2;
        stats["DEX"] += 2;
        stats["STR"] += 4;
        stats["LUCK"] += 2;
        stats["CHARM"] -= 3;
    }

    // Sex
    if (sex == "Male") {
        stats["STR"] += 1;
        stats["DEX"] -= 1;
        stats["LUCK"] += 1;
    }
    else if (sex == "Female") {
        stats["DEX"] += 1;
        stats["INT"] += 1;
        stats["CHARM"] += 2;
    }

    // Gender (just examples — balance however you like)
    if (gender == "Homosexual") {
        if (sex == "Male") {
            stats["CHARM"] += 2;
            stats["STR"] -= 1;
            stats["INT"] += 1;
            stats["LUCK"] -= 1;
        } else {
            stats["CHARM"] -= 2;
            stats["STR"] += 1;
            stats["INT"] -= 1;
            stats["DEX"] += 1;
        }
    } else if (gender == "Bisexual") {
        if (sex == "Female") {
            stats["LUCK"] += 1;
        } else {
            stats["CHARM"] += 2;
            stats["LUCK"] -= 1;
        }
        
    } else {
        stats["LUCK"] += 1;
        stats["CHARM"] -= 1;
    };

    // Profession
    if (profession == "Knight") {
        stats["STR"] += 3;
        stats["GIRTH"] += 2;
    } else if (profession == "Mage") {
        stats["INT"] += 4;
        stats["DEX"] -= 1;
    } else if (profession == "Archer") {
        stats["GIRTH"] -= 1;
        stats["DEX"] += 1;
    } else if (profession == "Thief") {
        stats["DEX"] += 2;
        stats["CHARM"] += 1;
    } else if (profession == "Clown") {
        // 1. Sum all stat values
        int total = 0;
        for (const auto& stat : stats) {
            total += stat.second;
        }
    
        // 2. Pick a random stat
        std::vector<std::string> keys;
        for (const auto& stat : stats) {
            keys.push_back(stat.first);
        }
        int randomIndex = RPGUtils::rollDie(static_cast<int>(keys.size())) - 1;
        std::string chosenStat = keys[randomIndex];
    
        // 3. Zero all stats
        for (auto& stat : stats) {
            stat.second = 0;
        }
    
        // 4. Assign total to chosen stat
        stats[chosenStat] = total;
    
        std::cout << "[CLOWN EFFECT] All stats combined into: " << chosenStat
                  << " = " << total << "\n";
    }
}
