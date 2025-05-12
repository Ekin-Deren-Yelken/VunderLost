//g++ main.cpp characters/character.cpp utils/rpg_utils.cpp combat/combat.cpp -o game
#include "../../include/rpg_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <filesystem>
#include <array>

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

    std::string findPythonInPath() {
        const char* pathEnv = std::getenv("PATH");
        if (!pathEnv) return "";

        std::stringstream ss(pathEnv);
        std::string segment;
        while (std::getline(ss, segment, ';')) {
            std::filesystem::path potential = segment;
            potential /= "python.exe";
            if (std::filesystem::exists(potential)) {
                return potential.string();
            }
        }

        return "";
    }

    std::string runSentimentAnalysis(const std::string& input) {
        std::string command = "python sentiment_check.py \"" + input + "\"";
        std::array<char, 128> buffer;
        std::string result;

        std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) return "neutral";
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }

        // Remove any trailing newline
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        return result;
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