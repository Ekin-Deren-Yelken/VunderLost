//g++ main.cpp characters/character.cpp utils/rpg_utils.cpp combat/combat.cpp -o game
#include "../../include/rpg_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>


namespace {
    std::mt19937& getRng() {
        static std::random_device rd; // static so only 1 dice roll, std to access standard library items, random_device to roll random number
        static std::mt19937 eng(rd());
        return eng;
    }
}
namespace RPGUtils {
    std::string getConfigValue(const std::string& key) {
        std::ifstream file("config.txt");
        std::string line;
        while (std::getline(file, line)) {
            auto sep = line.find('=');
            if (sep != std::string::npos) {
                std::string k = line.substr(0, sep);
                std::string v = line.substr(sep + 1);
                // Trim spaces
                k.erase(std::remove_if(k.begin(), k.end(), ::isspace), k.end());
                v.erase(std::remove_if(v.begin(), v.end(), ::isspace), v.end());
                if (k == key) return v;
            }
        }
        return "";
    }

    std::string runSentimentAnalysis(const std::string& sentence) {
        std::string pythonPath = getConfigValue("PYTHON_PATH");
        if (pythonPath.empty()) {
            std::cerr << "[ERROR] PYTHON_PATH not found in config.txt\n";
            return "error";
        }

        std::string cmd = "\"" + pythonPath + "\" sentiment_check.py \"" + sentence + "\"";

        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) return "error";

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            result += buffer;
        }
        _pclose(pipe);

        result.erase(result.find_last_not_of(" \n\r\t") + 1);
        return result.empty() ? "error" : result;
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