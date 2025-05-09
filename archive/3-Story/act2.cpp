#include "act2.h"
#include <iostream>

void runAct2(Character& player) {
    std::cout << "\n[ACT 2 begins for " << player.getDisplayName() << "]\n";
    // Your act 2 gameplay goes here...
    player.setAct(3);  // Advance to next act when done
}
