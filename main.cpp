#include <iostream>
#include "0-utils/rpg_utils.h"
#include "1-Characters/npc_factory.h"
#include "1-Characters/character.h"
#include "4-Saves/save_system.h"

#include "3-story/story.h"

#include "0-utils/json.hpp"
using json = nlohmann::json;

#include "3-story/act1.h"
#include "3-story/act2.h"
#include "3-story/act3.h"
#include "3-story/act4.h"

int main() {
    std::cout << "Welcome to the RPG\n[1] New Game\n[2] Load Game\n> ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    Character player;

    if (choice == 1) {
        runIntroScene();
    } else if (choice == 2) {
        if (!loadGame(player)) {
            std::cout << "Failed to load game. Starting new game.\n";
            runIntroScene();
        }
    } else {
        std::cout << "Invalid choice. Exiting.\n";
        return 0;
    }

    switch (player.getAct()) {
        case 1: runAct1(player); break;
        //case 2: runAct2(player); break;
        //case 3: runAct3(player); break;
        //case 4: runAct4(player); break;
        default:
            std::cout << "Unknown act. Starting from Act 1.\n";
            //runAct1(player);
            break;
    }

    return 0;
}


// int main(int, char**){
//     std::cout << "Hello, from VunderLost!\n";
//     std::cout << "Roll 3 D6!\n";
//     std::cout << RPGUtils::rollDice(3, 6) << "\n";

//     Character knight = createKnight();
//     Character goblin1 = createGoblin();
//     Character goblin2 = createGoblin();  // clone with same stats
//     Character slime = createSlime();


// }
