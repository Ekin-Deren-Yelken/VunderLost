#include "save_system.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "../../include/character.h"

namespace fs = std::filesystem;

std::string saveGame(const Character& player) {
    // Ensure target folder exists
    std::string folder = "gameSaves";
    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }

    // get saves in directory
    std::vector<fs::directory_entry> saves;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".json") {
            saves.push_back(entry);
        }
    }
    // if SaveID field is not empty, then we are overwriting
    std::string oldSaveID = player.getSaveID();
    if (!oldSaveID.empty()) {
        std::string oldFilename = folder + "/Save-" + oldSaveID + ".json";
        if (std::filesystem::exists(oldFilename)) {
            std::filesystem::remove(oldFilename);
            std::cout << "[Old save removed: " << oldFilename << "]\n";
        }
    }

    // Check that there only 5 saves maximum
    if (saves.size() >= 5) {
        std::cout << "[Save limit reached: 5 saves]\n";
        std::cout << "Would you like to overwrite one? (Y/N): ";
        std::string input;
        std::getline(std::cin, input);

        if (input != "Y" && input != "y") {
            std::cout << "Save aborted.\n";
            return "";
        }

        // List saves with numbers
        std::cout << "\nChoose a save to overwrite:\n";
        int index = 1;
        std::vector<std::string> filenames;
        for (const auto& save : saves) {
            std::cout << "[" << index << "] " << save.path().filename().string() << "\n";
            filenames.push_back(save.path().string());
            index++;
        }

        int choice = 0;
        while (true) {
            std::cout << "Enter number (1-5): ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice >= 1 && choice <= 5) break;
            std::cout << "Invalid input.\n";
        }

        // Load existing save to compare
        Character loaded;
        std::ifstream file(filenames[choice - 1]);
        if (file.is_open()) {
            json j;
            file >> j;
            loaded.loadFromJson(j);
        }

        std::cout << "\nYou are about to overwrite this character:\n";
        loaded.printStats();

        std::cout << "\nWith this character:\n";
        player.printStats();

        std::cout << "\nAre you sure you want to overwrite? (Y/N): ";
        std::string confirm;
        std::getline(std::cin, confirm);

        if (confirm != "Y" && confirm != "y") {
            std::cout << "Overwrite canceled.\n";
            return "";
        }

        std::ofstream out(filenames[choice - 1]);
        json j = player.toJson();
        out << j.dump(4);
        std::cout << "[Save overwritten: " << filenames[choice - 1] << "]\n";
        return filenames[choice - 1];
    }

    // Create new save with timestamp
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    std::ostringstream timeStr;
    timeStr << std::put_time(tm, "%Y-%m-%d-%H-%M-%S");
    std::string timestamp = timeStr.str();

    std::string filename = folder + "/Save-" + timestamp + ".json";

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to save file.\n";
        return "";
    }

    // Inject the saveID field into JSON
    json j = player.toJson();
    if (player.getController() == core::ControllerType::Human) {
        j["saveID"] = timestamp;
        const_cast<Character&>(player).setSaveID(timestamp);
    }

    file << j.dump(4);
    std::cout << "[New save created: " << filename << "]\n";
    return filename;
}


bool loadGame(Character& player) {
    std::string folder = "gameSaves";
    if (!fs::exists(folder)) {
        std::cout << "No save folder found.\n";
        return false;
    }

    std::vector<fs::directory_entry> saves;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".json") {
            saves.push_back(entry);
        }
    }

    if (saves.empty()) {
        std::cout << "No saves available.\n";
        return false;
    }

    // Sort saves by creation time (optional)
    std::sort(saves.begin(), saves.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            return fs::last_write_time(a) < fs::last_write_time(b);
        });

    std::cout << "\nAvailable saves:\n";
    for (size_t i = 0; i < saves.size(); ++i) {
        std::cout << "[" << std::setw(2) << std::setfill('0') << (i + 1) << "] " << saves[i].path().filename().string() << "\n";
    }

    int choice = 0;
    while (true) {
        std::string input;
        std::cout << "Enter number (1-" << saves.size() << "): ";
        std::getline(std::cin, input);

        // Check if all characters are digits
        if (std::all_of(input.begin(), input.end(), ::isdigit)) {
            choice = std::stoi(input);
            if (choice >= 1 && choice <= static_cast<int>(saves.size())) {
                break;
            }
        }
    
        std::cout << "Invalid input. Please enter a number between 1 and " << saves.size() << ".\n";
    }

    std::ifstream file(saves[choice - 1].path());
    if (!file.is_open()) {
        std::cerr << "Failed to open save file.\n";
        return false;
    }

    json j;
    file >> j;
    player.loadFromJson(j);

    std::cout << "[Game loaded: " << saves[choice - 1].path().filename() << "]\n";
    player.printStats();
    return true;
}
