#include "npc_factory.h"

Character createKnight() {
    return Character("Sir Roland", "Human", "Knight", "Male", "Straight", 5, 0, 1, 0);
}

Character createGoblin() {
    return Character("Goblin", "Beast", "Thief", "Male", "Straight", 2, 0, 1, 0);
}

Character createSlime() {
    Character slime = Character("Slime", "Beast", "Clown", "Unknown", "Unknown", 1, 0, 1, 0);
    return slime;
}