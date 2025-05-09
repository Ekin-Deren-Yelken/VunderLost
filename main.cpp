#include <iostream>

#include "include/character.h"
#include "include/story.h"
#include "include/act1.h"
#include "include/save_system.h"

#include "JSON/json.hpp"
using json = nlohmann::json;

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
        case 2: //runAct2(player); break;
        case 3: //runAct3(player); break;
        case 4: //runAct4(player); break;
        default:
            std::cout << "Unknown act. Starting from Act 1.\n";
            runAct1(player);
            break;
    }

    return 0;
}