#ifndef RPG_UTILS_H
#define RPG_UTILS_H

#include <random>
#include <string>

namespace RPGUtils {
    // Roll one die
    int rollDie(int sides);
    // Roll multiple dice with the same number of sides
    int rollDice(int count, int sides);
    // Roll multiple dice with different numbers of sides
    int rollDice(const std::vector<int>& sidesList);
    /// Convenience overload so you can write rollDice({6,8,10})
    int rollDice(std::initializer_list<int> sidesList);
    
    std::string runSentimentAnalysis(const std::string& sentence);
}

#endif