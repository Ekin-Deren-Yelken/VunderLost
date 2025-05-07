#include "act1.h"
#include "../1-Characters/character.h"
#include "../5-Combat/CombatManager.h"
#include "../0-utils/json.hpp"
#include "story.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>

using json = nlohmann::json;

Character* loadMonster(const std::string& relPath) {
    using json = nlohmann::json;

    auto fullPath = std::filesystem::current_path() / relPath;
    std::ifstream in(fullPath);

    // Check if JSON open
    if (!in.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return nullptr;
    }

    // Create JSON Stream buffer to protect data
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string jsonStr = buffer.str();

    // Check if empty
    if (jsonStr.empty()) {
        std::cerr << "ERROR: File content is empty.\n";
        return nullptr;
    }

    // Remove BOM chars if present
    if (jsonStr.size() >= 3 &&
        (unsigned char)jsonStr[0] == 0xEF &&
        (unsigned char)jsonStr[1] == 0xBB &&
        (unsigned char)jsonStr[2] == 0xBF) {
        jsonStr.erase(0, 3);
    }

    // Parse JSON
    json monster_json;
    try {
        monster_json = json::parse(jsonStr);
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return nullptr;
    }

    // Assign data from JSON to Monster Character, using pointer
    Character* monster = new Character();
    try {
        monster->loadFromJson(monster_json);
    } catch (const json::exception& e) {
        std::cerr << "Failed to load character from JSON: " << e.what() << std::endl;
        delete monster;
        return nullptr;
    }
    return monster;
}

void runAct1(Character& player, Character& roland) {
    std::cout << "\n[ACT 1 begins for " << player.getDisplayName() << "]\n";

    std::cout << "\nYou slowly regain consciousness as distant screams echo in the corridors.\n";
    std::cout << player.getName() << " blinks and realizes their cell door stands ajar.\n";
    std::cout << "Heart pounding, you peer out into the dimly lit hallway...\n\n";

    std::cout << "Before you can step forward, a hideous SLIME lunges from the shadows!\n";
    std::cout << "It oozes toward you, acidic tendrils whipping in the torchlight!\n\n";

    waitForEnter();
    auto* slime = loadMonster("5-Combat/data/slime.json");

    std::cout << slime->getDisplayName()
          << ":\n\n";

    std::cout << "\"Humpty dum, humpty dee,\n"
            << "Who dares disturb the slimey me?\n"
            << "Flesh and bone don't stand a chance-\n"
            << "Come now, stranger, let us DANCE!\"\n\n";

    waitForEnter();

    // Your party is just the player
    std::vector<Character*> players = { &player };
    std::vector<Character*> enemies;

    if (slime) enemies.push_back(slime);
    
    combat::CombatManager cm;
    cm.startEncounter(players, enemies);
    waitForEnter();
    for (auto* enemy : enemies) delete enemy;
    //player.setAct(2);  // Advance to next act when done
}
