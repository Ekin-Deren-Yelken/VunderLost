#include <iostream>
#include <fstream>
#include <string>

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits>
#include "story.h"
#include "../4-Saves/save_system.h"
#include "../1-Characters/stat_utils.h"
#include "0-utils/rpg_utils.h"
#include "act1.h"

// Helper: Wait for Enter
void waitForEnter() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper: Get input from player
std::string getInput(const std::string& prompt, bool toLower = false) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    if (toLower) {
        std::transform(input.begin(), input.end(), input.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }
    return input;
}

std::string toLowerStr(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Main intro logic
void runIntroScene() {
    // Dialogue 1
    std::cout << "You wake up on the hard surface of what you assume to be a wooden cart as it bumps along a dirt road. A hood covers your eyes but can make out shapes...\n";
    waitForEnter();

    // Dialogue 2
    std::cout << "A knightly figure bellows \"What is your name, scum?\" As you fade out of your daze, you mumble: \"My name is \" ";
    std::string playerName = getInput("> ", false);
    std::cout << "\nYou hear the knight scribbling something down as you pass out again...\n";
    waitForEnter();

    // Dialogue 3
    std::cout << "\nYou wake up again looking around and see a mirror and a bed in a stone cell.\n";
    std::cout << "A small window slit lets some moonlight through, and a thick steel cell door separates you from the hallway.\n";
    
    // Create player Character
    Character player(playerName, "Human", "Knight", "Male", "Straight", 0, 0, 1, 1);
    player.setController(core::ControllerType::Human);
    std::map<std::string, int> baseStats = getBaseStats();

    for (const auto& [key, value] : baseStats) {
        player.setStat(key, value);
    }
    //player.printStats();
    loopUntilMirrorChoice(player);
}

// Introduction to Interactive text
void loopUntilMirrorChoice(Character& player) {
    int invalidCount = 0;
    int doNothingCount = 0;
    bool bedChecked = false;
    bool doorChecked = false;
    int doorDamage = 0;
    int windowDamage = 0;
    int damage_from_stupidity = 0;

    // Action 1
    while (true) {
        std::cout << "\n" << player.getName();
        std::string choice = getInput(" decides to approach the \n> ", true);
        
        // Mirror Option
        if (choice.find("mirror") != std::string::npos) {
            std::cout << "\nYou cautiously approach the mirror...\n";
            std::cout << player.getName() << " holds the mirror up to their face...\n";
            break;
        }
        //Door Option
        else if (choice.find("door") != std::string::npos) {
            std::cout << "\nYou turn your attention to the cell door and approach the thick frame... ";
            if (!doorChecked) {
                std::string choiceDoor = getInput("maybe you can break it down... \nDo you trust your strength and ram into the door? (y/n) \n> ", true);
                if (choiceDoor == "yes" || choiceDoor == "y") {
                    std::cout << player.getDisplayName() << " backs up and charges at the door with all their might...\n";
                    std::cout << " Rolling for Strength...";
                    doorDamage = RPGUtils::rollDie(4) + 1;
                    damage_from_stupidity += doorDamage;
                    std::cout << player.getDisplayName() << " rams into a solid steel door taking " << doorDamage << " physical damage. \n\n";

                    std::cout << "Note from Developer: Congrats, the game just started and you ran straight into a solid steel door...\n";
                    waitForEnter();

                    std::cout << "\nTitle Unlocked: \"The Stupid\"";
                    player.addTitle("The Stupid");
                } else if (choiceDoor == "no" || choiceDoor == "n") {
                    std::cout << player.getDisplayName() << ": Hm, nothing of note here...\n";
                }
                doorChecked = true;
            } else {
                std::cout << player.getDisplayName() << ": I already checked here, nothing to see...\n";
            }
            break;
        }
        // Bed Option
        else if (choice == "bed") {
            if (!bedChecked) {
                std::cout << "\nYou see poop stains on the mattress. What a mess you made, how embarassing... Shamefully, you walk back to where you woke up.";
                bedChecked = true;
                std::cout << "\nTitle Unlocked: \"The Stinky\"";
                player.addTitle("The Stinky");
                continue;
            } else if (bedChecked) {
                std::cout << "\nYou already checked the bed, stinker. Nothing has changed.";
                continue;
            }
        }
        //Window Option
        else if (choice.find("window") != std::string::npos) {
            std::cout << "\nYou approach the window its quite high... ";
            if (!doorChecked) {
                std::string choiceDoor = getInput("maybe you can jump real high and see through it... \nDo you trust your dexterity and jump up to see through? (y/n) \n> ", true);
                if (choiceDoor == "yes" || choiceDoor == "y") {
                    std::cout << player.getDisplayName() << " runs and jumps with all their might...\n";
                    std::cout << " Rolling for Dexterity...";
                    windowDamage = RPGUtils::rollDie(3) + 1;
                    damage_from_stupidity += windowDamage;
                    std::cout << player.getDisplayName() << " smashes misses the landing and hits the wall taking " << windowDamage << " physical damage. \n\n";
                    waitForEnter();
                } else if (choiceDoor == "no" || choiceDoor == "n") {
                    std::cout << player.getDisplayName() << ": Too high up to see through...\n";
                }
                doorChecked = true;
            } else {
                std::cout << player.getDisplayName() << ": I already checked here, nothing to see...\n";
            }
            break;
        }
        // Do Nothing Option
        else if (choice == "do nothing") {
            doNothingCount++;
            if (doNothingCount == 1)
                std::cout << "You do nothing...\n";
            else if (doNothingCount == 2)
                std::cout << "You stare at the ceiling blankly... again.\n";
            else if (doNothingCount == 3)
                std::cout << "You're truly committed to doing absolutely nothing, huh?\n";
            else if (doNothingCount == 4)
                std::cout << "You wonder if 'mirror' was too subtle a hint...\n";
            else {
                std::cout << "\nYou feel a fart coming and let it out. Unfortunatly for you, its diarrhea. There is no cure. You wither away in irrelevance and your feces and die.\n";
                std::cout << "Restarting game...\n\n";
                runIntroScene();  // Restart
                return;
            }
        }
        else if (choice.find("search") != std::string::npos) {
            std::cout << "You find nothing else of note in the room...\n\n";
        }

        // Invalid Input Option
        else {
            invalidCount++;
            std::cout << "You look around and try to \"" << choice << "\", but fail. You blush in embarrassment...\n";
            waitForEnter();
            if (invalidCount >= 2)
                std::cout << "(Hint: try typing 'bed', 'mirror', or 'do nothing')\n";
        }
    }
    runCharacterSelection(player);

}

// Set Player Stats
void runCharacterSelection(Character& player) {

    // 1. Race selection
    const std::vector<std::string> validRaces = { "human", "elf", "beast" };
    std::string race;
    while (true) {
        race = getInput("\nLooking into the mirror you see that you are part of the race of (Human, Elf, Beast): ", true);
        if (std::find(validRaces.begin(), validRaces.end(), race) != validRaces.end()) {
            player.setRace(race);  // save lowercase version
            break;
        } else {
            std::cout << "There is no such race as the \"" << race << "\".\n";
        }
    }

    // 2. Sex selection
    const std::vector<std::string> validSexes = { "male", "female" };
    std::string sex;
    while (true) {
        sex = getInput("\nAs your eyes clear, you look down at your tattered clothing and notice your sex (Male or Female): ", true);
        if (std::find(validSexes.begin(), validSexes.end(), sex) != validSexes.end()) {
            player.setSex(sex);
            break;
        } else {
            std::cout << "In this world, you need to have one organ or the other...\n";
        }
    }

    // 3. Gender preference
    const std::vector<std::string> validGenders = { "straight", "homosexual", "bisexual" };
    std::string gender;
    while (true) {
        gender = getInput("\nAh yes, a " + sex + " " + race + "... but when it comes to your preferences in bed, you remember you are (Straight, Homosexual, Bisexual): ", true);
        if (std::find(validGenders.begin(), validGenders.end(), gender) != validGenders.end()) {
            player.setGender(gender);
            break;
        } else {
            std::cout << "In this world, we are not advanced past these three options. Pick one.\n";
        }
    }

    // 4. Selection Confirmation
    std::cout << "\n" << player.getDisplayName()
              << " of the " << player.getRace()
              << "s. A " << player.getGender()
              << " " << player.getSex() << ".\n";

    waitForEnter();

    runProfessionSelection(player);
}

void runProfessionSelection(Character& player) {
    // Dialogue 1
    std::cout << "You hear footsteps approaching your cell... knightly footsteps.\n\n";

    // Create Roland NPC for later...
    Character roland(
        "Sir Roland",
        "Human",
        "Knight",
        "Male",
        "Homosexual",
        /*armour*/5,
        /*XP*/0,
        /*level*/1,
        /*act*/1
    );
    roland.addTitle("The Rigid Oak");
    roland.setController(core::ControllerType::AI);

    // Dialogue 2
    std::cout << "\n" << roland.getDisplayName()
          << ": \"Get up, " << player.getName()
          << "! Follow me into the council chamber.\n";
    waitForEnter();

    // Confirm Profession
    while (true) {
        std::cout << "\n" << roland.getDisplayName() << ": On the table lie five items we recovered from the forest, which one belongs to you scum:\n"
                << " [1] Sword   (Knight)\n"
                << " [2] Ring    (Mage)\n"
                << " [3] Dagger  (Thief)\n"
                << " [4] Quiver  (Archer)\n"
                << " [5] Wig     (Clown)\n"
                << " [6] Seduce Roland\n"
                << "> ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        std::string chosenClass, item;
        switch (choice) {
        case 1: chosenClass="Knight"; item="sword";  break;
        case 2: chosenClass="Mage";   item="ring";   break;
        case 3: chosenClass="Thief";  item="dagger"; break;
        case 4: chosenClass="Archer"; item="quiver"; break;
        case 5: chosenClass="Clown";  item="wig";    break;
        case 6: //roll to see if it works.
        default:
            std::cout << roland.getDisplayName() << ": \"Pathetic. Speak clearly, which of these belongs to you.\"\n";
            continue;
        }
        
        std::cout << roland.getDisplayName() << ": \"A " << chosenClass << "? I knew it. Now confirm.\"\n";
        std::string confirm = getInput("Confirm your choice? (y/n): ", true);

        if (confirm == "y" || confirm == "yes") {
            std::cout << player.getName() << " snatches up the " << item << ".\n";
            
        // Initialize Character (create wrapper later)
        player.setProfession(chosenClass);

        std::map<std::string, int> playerStats = applyStatModifiers(getBaseStats(), toLowerStr(player.getRace()), toLowerStr(player.getSex()), toLowerStr(player.getGender()), toLowerStr(player.getProfession()));

        for (const auto& [stat, val] : playerStats) {
            player.setStat(stat, val);
        }

        // Now re-set health/mana based on new stats
        player.calculateVitals();
        player.setHealth(player.getMaxHealth());
        player.setMana(player.getMaxMana());
        player.setAct(1);  

        waitForEnter();
            break;
        }

        std::cout << roland.getDisplayName() << ": \"Hesitant? Then choose again!\"\n";
    }

    // Dialogue 3
    std::cout << roland.getDisplayName()
          << ": \"I almost forgot... one last gift.\"\n";
    waitForEnter();

    // Dialogue 4
    std::cout << "Before you can react, " << roland.getName() << " winds up punches you... WHACK!\n\n";
    std::cout << "Stars burst in your vision. Then darkness.\n";
    std::cout << "You collapse, unconscious... again.\n";
    waitForEnter();

    // Print Stats
    player.printStats();

    // Save Character?
    std::string choice = getInput("Would you like to save your character? (yes/no): ", true);
    for (auto &c : choice) c = std::tolower(c);
    while (true) {
        if (choice == "yes" || choice == "y") {
            saveGame(player);  // uses timestamped filename
            break;
        } else if (choice == "no" || choice == "n"){
            std::cout << "Continuing without saving...\n";
            break;
        }  else {
            std::cout << "Please Type Valid Input.";
        }
    }

    // Start Act 1
    std::string prompt = std::string("Start Act ") 
                   + std::to_string(player.getAct()) 
                   + " (y/n): ";

    std::string confirmAct1 = getInput(prompt, true);
    for (auto &c : confirmAct1) c = std::tolower(c);
    while (true) {
        if (confirmAct1 == "yes" || confirmAct1 == "y") {
            runAct1(player);
            break;
        } else if (confirmAct1 == "no" || confirmAct1 == "n"){
            break;
        }  else {
            std::cout << "Please Type Valid Input.";
        }
    }
}