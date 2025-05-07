#include "act1.h"
#include "../1-Characters/character.h"
#include "../5-Combat/CombatManager.h"
#include "../0-utils/rpg_utils.h"
#include "../0-utils/json.hpp"
#include "story.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <string>
#include <cstdio>

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
    waitForEnter();

    std::cout << "\nYou slowly regain consciousness as distant screams echo in the corridors.\n";
    std::cout << player.getName() << " blinks and realizes their cell door stands ajar as you hear movement in the dimly lit hall...\n";
    waitForEnter();

    std::cout << "Before you can step forward, a hideous SLIME lunges from the shadows!\n";
    std::cout << "It oozes toward you, with confidence and style!\n\n";

    auto* slime = loadMonster("5-Combat/data/slime.json");

    waitForEnter();

    std::cout << slime->getDisplayName()
              << ":\n\n"
              << "\"Humpty dum, humpty dee,\n"
              << "Who dares disturb the slimey me?\n"
              << "Flesh and bone don't stand a chance-\n"
              << "Come now, stranger, let us DANCE!\"\n\n";
    waitForEnter();

        // Your party is just the player
        std::vector<Character*> players = { &player };
        std::vector<Character*> enemies;
    
        if (slime) enemies.push_back(slime);
        
        combat::CombatManager cm;
        bool gregoryBattle = cm.startEncounter(players, enemies);
        waitForEnter();

    if (gregoryBattle) {
        // Greg Won
        std::cout << slime->getDisplayName() << ": \"Another one bites the goo!\"\n";
        std::cout<< slime->getDisplayName() << ": Honestly, I expected more from you. Someone with limbs lost to me; sadly true.\"\n";
        waitForEnter();
        std::cout << slime->getDisplayName() << ": Now if you'll excuse me, I must moonwalk into the abyss. *squelch*\"\n";
        waitForEnter();
        std::cout << slime->getDisplayName() << "As you lie in defeat that you just lost to a rhymin', dancin' ball of goo, you hear a familiar voice...\n";
    } else {
        // player won
        std::cout << slime->getDisplayName() << ": \"No... my rhythm, my groove... this can't be true!\"\n";
        waitForEnter();
        std::cout << slime->getDisplayName() << ": \"I did't even *try*. You could never handle the slimey style!\"\n";
        waitForEnter();
    }
    for (auto* enemy : enemies) delete enemy;

    bool gainGreg = false;

    std::string henchSex;
    if (player.getSex() == "Male") {henchSex="man"; } else {henchSex="woman";}

    std::cout << slime->getDisplayName() << ": How about we team up, you could be my hench" << henchSex
                                         <<  ". As a poet and dancer, far superior to you, I demand a lengthy and detailed acceptance to my offer...\n";

    bool sentimentChecked = false;

    while (!sentimentChecked) {
        std::string hechmenChoice = getInput("what do you say > ", true);
        std::string cmd = "python sentiment_check.py \"" + hechmenChoice + "\"";
    
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) { std::cerr << "Failed to run sentiment check.\n"; return; }
    
        char buffer[128];
        std::string sentiment = RPGUtils::runSentimentAnalysis(hechmenChoice);
        if (sentiment == "hostile") {
            std::cout << slime->getName() << "recoils in gooey anger.\n";
            sentimentChecked = true;
        } else if (sentiment == "friendly") {
            std::cout << slime->getName() << "bounces happily.\n";
            std::cout <<  slime->getDisplayName() << " has joined your team...\n";
            sentimentChecked = true;
        } else {
            std::cout << slime->getName() << "... is confused.\n";
            std::cout << slime->getDisplayName() << ": speak clearly fool, be as descriptive as you like,"
                     << " a poet and artist of my calibre, far outside your liguistic capabilities, can "
                     << "take any length of answer you decide is necessary\n";
        }
    }

}