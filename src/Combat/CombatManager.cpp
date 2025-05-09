#include "CombatManager.h"
#include "../../include/rpg_utils.h"    // for rollDice, chanceRoll
#include "../../include/AbilityLoader.h"
#include "../../include/Effect.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>


using namespace combat;
using namespace core;

std::unique_ptr<Character> combatManager::loadMonster(const std::string& relPath) {
    using json = nlohmann::json;

    auto fullPath = std::filesystem::current_path() / relPath;
    std::ifstream in(fullPath);

    // Check if JSON open
    if (!in.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << "\n";
        return nullptr;
    }

    // Create JSON Stream buffer to protect data
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string jsonStr = buffer.str();

    // Check if empty
    if (jsonStr.empty()) {
        std::cerr << "ERROR: File content is empty.\n";
        return nullptr;
    }

    // Remove BOM chars if present
    if (jsonStr.size() >= 3 &&
        (unsigned char)jsonStr[0] == 0xEF &&
        (unsigned char)jsonStr[1] == 0xBB &&
        (unsigned char)jsonStr[2] == 0xBF) {
        jsonStr.erase(0, 3);
    }

    // Parse JSON
    json monster_json;
    try {
            monster_json = json::parse(jsonStr);
    }
    catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return nullptr;
    }

    // Assign data from JSON to Monster Character, using unique pointer
    auto monster = std::make_unique<Character>();
    try {
        monster->loadFromJson(monster_json);
    } catch (const json::exception& e) {
        std::cerr << "Failed to load character from JSON: " << e.what() << std::endl;
        return nullptr;
    }
    return monster;
}

bool combatManager::startEncounter(std::vector<Character*>& players,
                                   std::vector<Character*>& enemies) {
    // build turn order: players then enemies
    allCombatants.clear();
    for (auto* p : players) allCombatants.push_back(p);
    for (auto* e : enemies) allCombatants.push_back(e);

    // Load abilities of all the combatants
    loadAbilities();
    activeIndex = 0;

    // loop until one side is wiped out, all players or enemies health is reduced to zero
    while (true) {

        // Run Next Turn
        nextTurn();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Check if players or enemies are all dead
        bool anyPlayerAlive = std::any_of(players.begin(), players.end(), [](Character* c){ return c->isAlive(); });
        bool anyEnemyAlive = std::any_of(enemies.begin(), enemies.end(), [](Character* c){ return c->isAlive(); });

        // If there are both enemies and players to battle, next turn
        if (!anyPlayerAlive || !anyEnemyAlive) break;

        std::cout << "\n-Next Combatant's Turn-\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    bool victory = std::any_of(enemies.begin(), enemies.end(), [](Character* c){ return c->isAlive(); });

    std::cout << (victory? "Defeat...\n\n" : "Victory!\n\n") << std::endl;
    
    return victory;
}

void combatManager::loadAbilities() {
    characterAbilities.clear();
    // Pull in the static map that was built once from your JSON file
    const auto& dict = getAllAbilities();
    for(Character* c : allCombatants) { 
        std::string prof = c->getProfession();                  //get profession for each combatant
        std::vector<Ability> aList;                             // List of abilities
        for (const auto& [id, ability] : dict) {                // For all the abilities in the dictionary of all abilities
            if(ability.profession == prof) {                    // check if ability has profession field
                aList.push_back(ability);                       // if it does, push it
            }
        }
        characterAbilities[c] = aList;
        if (c->getProfession() == "Human") {
            std::cout << "Loaded " << aList.size() << " abilities for " << c->getName() << " (" << prof << ")\n";
        }
    }
    
}

void combatManager::nextTurn() {
    Character* actor = allCombatants[activeIndex]; // Actor is the who has the turn and they are doing something
    if (!actor->isAlive()) {
        std::cout << actor->getName() << " has been defeated, skipping...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // skip dead combatants, get next available player or enemy
    } else if (actor->isPlayer() || actor->getController()==ControllerType::Summoned) {
        playerTurn(*actor);
    } else {
        enemyTurn(*actor);
    }
    endTurnCleanup();
    activeIndex = (activeIndex + 1) % allCombatants.size();
}

void combatManager::playerTurn(Character& p) {
    std::cout << "\n-- " << p.getName() << "'s turn --";
    p.printCombatStats();
    std::cout << "\n";

    auto it = characterAbilities.find(&p);
    if (it == characterAbilities.end() || it->second.empty()) {
        std::cout << "No usable abilities.\n";
        return;
    }
    const auto& myAbilities = it->second;

    // Filter Abilities based on current Mana
    std::vector<Ability> usableAbilities;
    for (const auto& a : myAbilities) {
        if (a.canUse(p.getCurrentMana())) {
            usableAbilities.push_back(a);
        }
    }

    // Edge Case: No Mana
    if (usableAbilities.empty()) {
        std::cout << "You do not have enough mana to use any abilities.\n";
        return;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Show ability list
    for (size_t i = 0; i < usableAbilities.size(); ++i) {
        std::cout << "[" << i << "] " << usableAbilities[i].name
                  << " (" << usableAbilities[i].cost << " mana)\n";
    }

    // Choose Ability
    std::cout << "Choose an ability to use > ";
    size_t abilityChoice;
    std::cin >> abilityChoice;
    const Ability& chosen = usableAbilities[abilityChoice];

    // Ensure Spell hits correct target
    if (chosen.abilityTarget == "self") {
        // Resolve with self target
        resolveAbility(p, p, chosen);
    } else {
        // Build enemy target list
        std::vector<Character*> possibleTarget;
        for (Character* c : allCombatants) {
            if (c->getController() == ControllerType::AI && c->isAlive()) {
                possibleTarget.push_back(c);
            }
        }
        
        // Valid Targets?
        if (possibleTarget.empty()) {
            std::cout << "There are no valid targets.\n";
            return;
        }
        // Player Target Choice
        std::cout << "\nAvailable targets: ";
        for (size_t i = 0; i < possibleTarget.size(); ++i) {
            std::cout << "[" << i << "] " << possibleTarget[i]->getName() << "    ";
            possibleTarget[i]->printCombatStats();
            std::cout;
        }
        // Get Choice
        std::cout << "Choose a Target > ";
        size_t targetChoice;
        std::cin >> targetChoice;
        Character* target = possibleTarget[targetChoice];

        // Resolve
        resolveAbility(p, *target, chosen);
    }

}

void combatManager::enemyTurn(Character& e) {
    // Simple AI: use first ability on first alive player or summoned ally
    
    // Find abilities corresponding to the enemy. Each entry in character Abilities is specific to that character.
    // This could be inefficient but I am only planning ot have maximum 8 characters with 5 abilities each. I think the size makesit reasonable.
    auto it = characterAbilities.find(&e);
    if (it == characterAbilities.end() || it->second.empty()) {
        std::cout << e.getName() << " has no abilities.\n";
        return;
    }
    const auto& enemyAbilities = it->second;

    // Filter for usable abilities
    std::vector<Ability> usable;
    for (const auto& a : enemyAbilities) {
        if (a.canUse(e.getCurrentMana())) usable.push_back(a);  // Mana available
    }

    if (usable.empty()) {
        std::cout << e.getName() << " has no mana to use any abilities.\n";
        return;
    }

    // Chose random abilitiy
    int idx = RPGUtils::rollDice(1, usable.size()) - 1;
    const auto& chosen = usable[idx];

    // Ensure correct target
    if (chosen.abilityTarget == "self") {
        resolveAbility(e, e, chosen);
    } else {
        // Targetting Rule:
        // Focus on Characters who are Summond first
        // If an opponent is below 25% health
        Character* target = chooseAITarget();
        if (!target) {
            std::cout << e.getName() << " finds no valid target.\n";
            return;
        }
        
        resolveAbility(e, *target, chosen);
    }
    
}

Character* combatManager::chooseAITarget() {
    Character* lowestHP = nullptr;
    Character* firstSummoned = nullptr;
    int minHP = INT_MAX;

    for (Character* c : allCombatants) {
        if ((c->getController() == ControllerType::Human || c->getController() == ControllerType::Summoned) && c->isAlive()) {
            // Track lowest HP target
            if (c->getCurrentHealth() < minHP) {
                minHP = c->getCurrentHealth();
                lowestHP = c;
            }

            // Track first summoned
            if (!firstSummoned && c->getController() == ControllerType::Summoned) {
                firstSummoned = c;
            }
        }
    }

    if (!lowestHP) return nullptr; // no valid targets

    float hpPercent = static_cast<float>(lowestHP->getCurrentHealth()) / lowestHP->getMaxHealth();

    if (hpPercent < 0.25f && firstSummoned) {
        int roll = RPGUtils::rollDice(1, 2);
        return (roll == 1) ? lowestHP : firstSummoned;
    }

    return firstSummoned ? firstSummoned : lowestHP;
}

void combatManager::resolveAbility(Character& user, Character& target, const core::Ability& a) {
    std::cout << user.getName() << " uses " << a.name << " on " << target.getName() << std::endl;
    
    //Spend Mana
    user.setHealthMana("M", -a.cost);

    // 1) Roll to hit
    int toHit = RPGUtils::rollDice(a.hitRoll.count, a.hitRoll.sides)
                + a.hitRoll.bonus;

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Roll to hit:" << " you need " << a.hitThreshold << "\n\n Rolling Dice..."; // TLDR; DISPLAY ODDS HERE
    std::this_thread::sleep_for(std::chrono::seconds(1));



    if (toHit < a.hitThreshold) {
        std::cout << "\nIt misses! (Rolled " << toHit
                  << ", needed " << a.hitThreshold << ")\n";
        return;
    }

    // 2) Roll damage
    std::cout << "\nIt Hits!" << " Calculating Damage... \n\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int damage = RPGUtils::rollDice(a.damageRoll.count, a.damageRoll.sides) + a.damageRoll.bonus;
    target.applyInstantDamage(damage, a.dType, a.name);  // damageRollType = new field if needed

    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "\n Applying Status Effects...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 3) Apply any status effects
    for (auto& e : a.effects) {
        if (e.status != core::StatusType::None)
            target.applyEffect(e, a.name);
    }

     // 5) Show the flavor comment
    if (!a.comment.empty()) {
       std::cout << a.comment << "\n\n";
    }

    // (Optional) QTE hook here for bonus
}

void combatManager::endTurnCleanup() {
    for (auto* c : allCombatants)         c->tickStatuses();

}
