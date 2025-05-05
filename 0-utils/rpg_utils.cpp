//g++ main.cpp characters/character.cpp utils/rpg_utils.cpp combat/combat.cpp -o game
#include "rpg_utils.h"


namespace {
    std::mt19937& getRng() {
        static std::random_device rd; // static so only 1 dice roll, std to access standard library items, random_device to roll random number
        static std::mt19937 eng(rd());
        return eng;
    }
}


// Returns an int, takes an int 'sides', in RPGUtils namespace
int RPGUtils::rollDie(int sides) {
    std::uniform_int_distribution<int> dist(1, sides);
    return dist(getRng());
}

int RPGUtils::rollDice(int count, int sides) {
    int total = 0;
    for (int i = 0; i < count; ++i)
        total += rollDie(sides);      // this calls RPGUtils::rollDie
    return total;
}

int RPGUtils::rollDice(const std::vector<int>& sidesList) {
    int total = 0;
    for (int s : sidesList)
        total += rollDie(s);
    return total;
}

int RPGUtils::rollDice(std::initializer_list<int> sidesList) {
    return rollDice(std::vector<int>(sidesList));
}