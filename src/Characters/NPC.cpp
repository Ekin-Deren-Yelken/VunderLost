#include "NPC.h"

#include <iostream>

NPC::NPC() {}

void NPC::setFavour(int value) {
    favour = value;
}

int NPC::getFavour() const {
    return favour;
}

void NPC::adjustFavour(int delta) {
    favour += delta;
}

bool NPC::isFriendly() const {
    return favour >= 10;  // Arbitrary threshold
}

void NPC::loadFromJson(const nlohmann::json& j) {
    Character::loadFromJson(j);  // Load shared character data

    try {
        if (j.contains("favour") && j["favour"].is_number_integer()) {
            favour = j["favour"];
        } else {
            std::cerr << "[NPC] Missing or invalid 'favour' value for NPC '" 
                      << getName() << "'. Defaulting to 0.\n";
            favour = 0;
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[NPC] JSON error while loading favour for '" 
                  << getName() << "': " << e.what() << "\n";
        favour = 0;
    }
}

