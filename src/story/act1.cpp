#include "../../include/act1.h"
#include "../../include/character.h"
#include "../../include/CombatManager.h"
#include "../../include/rpg_utils.h"
#include "../../JSON/json.hpp"
#include "../../include/story.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <string>
#include <cstdio>
#include <memory>

using json = nlohmann::json;

void runAct1(Character& player) {

    std::cout << "\n[ACT 1 begins for " << player.getDisplayName() << "]\n";
    waitForEnter();

    // Narrator
    std::cout << "\nYou slowly regain consciousness as distant screams echo in the corridors.\n";
    std::cout << player.getName() << " blinks and realizes their cell door stands ajar as you hear movement in the dimly lit hall...\n";
    waitForEnter();

    std::cout << "Before you can step forward, a hideous SLIME lunges from the shadows!\n";
    std::cout << "It oozes toward you, with confidence and style!\n\n";
    waitForEnter();

    // Initialize combat manager
    combat::combatManager cm;

    // Create enemies using smart pointer to avoid deleting later...
    std::unique_ptr<Character> gregory = cm.loadMonster("assets/mobs/NPC_GregoryLordOfGoo.json");

    // Dialogue 1
    std::cout << gregory->getDisplayName()
              << ":\n\n"
              << "\"Humpty dum, humpty dee,\n"
              << "Who dares disturb the slimey me?\n"
              << "Flesh and bone don't stand a chance-\n"
              << "Come now, stranger, let us DANCE!\"\n\n";
    waitForEnter();
    
    // use characters for combat
    std::vector<Character*> players = { &player };
    std::vector<Character*> enemies;

    // build enemies vector
    if (gregory) enemies.push_back(gregory.get());
    
    // Start Combat
    bool gregoryBattle = cm.startEncounter(players, enemies);
    waitForEnter();

    // Based on Combat Outcome, pick dialogue
    if (gregoryBattle) {
        // Greg Won
        std::cout << "\n"  << gregory->getDisplayName() << ": \"Another one bites the goo!\"\n";
        std::cout<< gregory->getDisplayName() << ": Honestly, I expected more from you. Someone with limbs lost to me; sadly true.\"\n";
        waitForEnter();
        std::cout << gregory->getDisplayName() << ": Now if you'll excuse me, I must moonwalk into the abyss. *squelch*\"\n";
        waitForEnter();
        std::cout << gregory->getDisplayName() << "As you lie in defeat that you just lost to a rhymin', dancin' ball of goo, you hear a familiar voice...\n";
    } else {
        // player won
        std::cout << "\n" << gregory->getDisplayName() << ": \"No... my rhythm, my groove... this can't be true!\"\n";
        waitForEnter();
        std::cout << gregory->getDisplayName() << ": \"I did't even *try*. You could never handle the slimey style!\"\n";
        waitForEnter();
    }

    // Dialogue 2
    bool gainGreg = false;
    std::string playerSex;
    if (player.getSex() == "male") {playerSex="man"; } else {playerSex="woman";}
    std::cout << gregory->getDisplayName() << ": How about we team up, you could be my hench" << playerSex
                                         <<  ". As a poet and dancer, far superior to you, I demand a lengthy and detailed acceptance to my offer...\n";

    // Check Sentiment of response, loop till positive or negative answer given
    bool sentimentChecked = false;
    int favour;

    while (!sentimentChecked) {
        std::string hechpersonChoice = getInput("what do you say > ", true);
        
        // Check Sentiment of response
        char buffer[128];
        std::string sentiment = RPGUtils::runSentimentAnalysis(hechpersonChoice);

        int favourRoll_gregory = RPGUtils::rollDie(10);

        // depending on sentiment, change response
        if (sentiment == "hostile") {
            std::cout << gregory->getName() << "recoils in gooey anger.\n";
            
            // Calculate Favour
            favour = player.getStat("CHARM") - favourRoll_gregory;

            gregory->setStat("favour", favour);
            sentimentChecked = true;
        } else if (sentiment == "friendly") {
            std::cout << gregory->getName() << "bounces happily.\n";

            // Calculate Favour
            favour = player.getStat("CHARM") + favourRoll_gregory;

            std::cout <<  gregory->getDisplayName() << " has joined your team...\n";
            sentimentChecked = true;
        } else {
            std::cout << gregory->getName() << "... is confused.\n";
            std::cout << gregory->getDisplayName() << ": speak clearly fool, be as descriptive as you like,"
                     << " a poet and artist of my calibre, far outside your liguistic capabilities, can "
                     << "take any length of answer you decide is necessary\n";
        }
    }
        std::cout << "\nHONK HONK";

}