#include "act3.h"
#include <iostream>

void runAct3(Character& player) {
    std::cout << "\n[ACT 3 begins for " << player.getDisplayName() << "]\n";
    // Your act 3 gameplay goes here...
    player.setAct(4);  // Advance to next act when done
}
