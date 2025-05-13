#include "CombatManager.h"
#include "../../include/rpg_utils.h"    // for rollDice, chanceRoll
#include "../../include/AbilityLoader.h"
#include "../../include/Effect.h"
#include "../../include/save_system.h"
#include "../../include/story.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>

using namespace combat;
using namespace core;

bool combatManager::startEncounter(std::vector<Character*>& players, std::vector<Character*>& enemies) {
    // build turn order: players then enemies
    allCombatants.clear();
    for (Character* p : players) {
        p->setTeam(core::Team::Players);
        allCombatants.push_back(p);
    }
    for (Character* e : enemies) {
        e->setTeam(core::Team::Enemies);
        allCombatants.push_back(e);
    }

    initGrid(players, enemies);

    // Load companions for all players and enemies
    std::vector<std::unique_ptr<Character>> loadedCompanions;

    auto loadAndAttachCompanions = [&](std::vector<Character*>& group) {
        std::vector<Character*> newCompanions;

        for (Character* leader : group) {
            if (leader->isCompanion()) {
                auto companions = loadCompanionParty(*leader);
                for (auto& c : companions) {
                    newCompanions.push_back(c.get());
                    allCombatants.push_back(c.get());
                    loadedCompanions.push_back(std::move(c));  // retain ownership
                }
            }
        }

        // Add new companions to the group (players or enemies)
        group.insert(group.end(), newCompanions.begin(), newCompanions.end());
    };

    // Apply to both teams
    loadAndAttachCompanions(players);
    loadAndAttachCompanions(enemies);

    // Load abilities of all the combatants
    loadAbilities();
    activeIndex = 0;

    // loop until one side is wiped out, all players or enemies health is reduced to zero
    int turnNumber = 1;

    while (true) {
        std::cout << "\n==== Turn " << turnNumber++ << " ====\n";
        // --- PLAYER PHASE ---
        std::cout << "\n-- PLAYER PHASE --\n";
        for (Character* p : players) {
            if (p->isAlive()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                printCombatGrid();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                playerTurn(*p);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "\n-Next Combatant's Turn-\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        // --- ENEMY PHASE ---
        std::cout << "\n-- ENEMY PHASE --\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (Character* e : enemies) {
            if (e->isAlive()) {
                printCombatGrid();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                enemyTurn(*e);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout << "\n-Next Combatant's Turn-\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        // Check for combat end
        bool anyPlayerAlive = std::any_of(players.begin(), players.end(), [](Character* c) { return c->isAlive(); });
        bool anyEnemyAlive = std::any_of(enemies.begin(), enemies.end(), [](Character* c) { return c->isAlive(); });

        if (!anyPlayerAlive || !anyEnemyAlive)
            break;

        endTurnCleanup();
    }

    // Determine Victory
    bool victory = std::any_of(enemies.begin(), enemies.end(), [](Character* c) { return c->isAlive(); });
    std::cout << (victory ? "Defeat...\n\n" : "Victory!\n\n") << std::endl;

    // Auto-save if players won
    if (!victory && !players.empty()) {
        saveGame(*players[0]);  // Save main player state
    }

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

void combatManager::initGrid(std::vector<Character*>& players, std::vector<Character*>& enemies) {
    // Initialize empty grid
    combatGrid = std::vector<std::vector<Character*>>(GRID_HEIGHT, std::vector<Character*>(GRID_WIDTH, nullptr));

    // Place players on left, enemies on right
    int px = 0, ex = GRID_WIDTH - 1;
    int y = 1;

    for (Character* p : players) {
        p->setPosition(px, y);
        combatGrid[y][px] = p;
        y += 2;
    }

    y = 1;
    for (Character* e : enemies) {
        e->setPosition(ex, y);
        combatGrid[y][ex] = e;
        y += 2;
    }

}

void combatManager::printCombatGrid() const {

    if (combatGrid.empty() || combatGrid[0].empty()) {
        std::cout << "(Grid not initialized)\n";
        return;
    }


    std::cout << "\n=== Combat Grid ===\n    ";

    // Print X-axis labels (A-H or however wide your grid is)
    for (int x = 0; x < GRID_WIDTH; ++x)
        std::cout << static_cast<char>('A' + x) << ' ';
    std::cout << "\n";

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        // Print Y-axis label (1-based)
        std::cout << std::setw(2) << y + 1 << "  ";

        for (int x = 0; x < GRID_WIDTH; ++x) {
            const Character* c = combatGrid[y][x];
            if (c) {
                if (c->getTeam() == core::Team::Players)
                    std::cout << 'P';
                else
                    std::cout << 'E';
            } else {
                std::cout << '.';
            }
            std::cout << ' ';
        }

        std::cout << "\n";
    }

    std::cout << "====================\n\n";
}

void combatManager::playerTurn(Character& p) {
    
    std::cout << "\n-- " << p.getName() << "'s turn --";
    p.printCombatStats();
    std::cout << "\n";

    bool goodAction = false;

    // Determine Abilities in Range
    std::cout << "Available Abilities within Range:\n";
    const auto& abilities = characterAbilities[&p];

    // Print Abilities in range
    for (size_t i = 0; i < abilities.size(); ++i) {
        const auto& a = abilities[i];
        bool inRange = false;

        // Self Targetting always in range
        if (a.abilityTarget == "self") {
            inRange = true;
        } else {
            // For every "enemy"
            for (auto* enemy : allCombatants) {
                // characters who are not on same team as player (p), who are alive, and who are in range of ability a
                if (enemy->getTeam() != p.getTeam() && enemy->isAlive() &&
                    isInRange(p, *enemy, a.range)) {
                    inRange = true;
                    break;
                }
            }
        }
        // Print ability if in range
        if (inRange) {
            std::cout << " > " << a.name << "\n";
        }
    }
    
    while (!goodAction) {
        // Chose action
        int action;
        std::cout << "\n\n===============\nChoose your action: \n[0] Move\n[1] Meditate\n[2] Use Ability\n\n" <<"> ";
        std::cin >> action;

            
        // Execute action
        if (action == 0) { 
            std::cout << "Choose Direction (N, NE, E, SE, S, SW, W, NW): ";
            std::string dir;
            std::cin >> dir;

            int dx = 0, dy = 0;
            if (dir == "N") dy = -1;
            else if (dir == "S") dy = 1;
            else if (dir == "E") dx = 1;
            else if (dir == "W") dx = -1;
            else if (dir == "NE") { dx = 1; dy = -1; }
            else if (dir == "NW") { dx = -1; dy = -1; }
            else if (dir == "SE") { dx = 1; dy = 1; }
            else if (dir == "SW") { dx = -1; dy = 1; }

            moveCharacter(p, dx, dy);
            goodAction = true;
        } else if (action == 1) {
            meditate(p);
            goodAction = true;
        } else if (action == 2) {
            useAbility(p);
            goodAction = true;
        } else {
            std::cout << "Invalid Input\n\n";
        }
    } // while loops

} // playerTurn()

void combatManager::enemyTurn(Character& e) {

    Character* target = chooseAITarget(e, e.getDifficulty());
    if (!target) {
        std::cout << e.getName() << " finds no valid target.\n";
        return;
    }

    // Determine AI decision
    float hpRatio = static_cast<float>(e.getCurrentHealth()) / e.getMaxHealth();
    int meditate50 = RPGUtils::rollDice(1, 2);
    int retreat15 = RPGUtils::rollDice(1, 2);
    
    // Step 1: 1/8 chance to meditate if under 50% HP
    if (hpRatio < 0.5 && meditate50 == 1) {
        std::cout << e.getName() << " chooses to meditate.\n";
        meditate(e);
        return;
    }

    // Step 1.5: 1/3 chance to retreat if under 15% HP
    else if (hpRatio < 0.15 && retreat15 < 2) {
        std::cout << e.getName() << " hesitates and retreats to recover.\n";

        int dex = e.getStat("DEX");
        int retreatSquares = 1;
        if (dex >= 15) retreatSquares = 3;
        else if (dex >= 10) retreatSquares = 2;

        Position myPos = e.getPosition();
        Position tPos = target->getPosition();
        int dx = (myPos.x > tPos.x) ? 1 : (myPos.x < tPos.x) ? -1 : 0;
        int dy = (myPos.y > tPos.y) ? 1 : (myPos.y < tPos.y) ? -1 : 0;

        bool retreated = false;
        for (int i = 1; i <= retreatSquares; ++i) {
            int newX = myPos.x + (dx * i);
            int newY = myPos.y + (dy * i);




            if (newX >= 0 && newX < GRID_WIDTH && newY >= 0 && newY < GRID_HEIGHT && combatGrid[newY][newX] == nullptr) {
                combatGrid[myPos.y][myPos.x] = nullptr;
                combatGrid[newY][newX] = &e;
                e.setPosition(newX, newY);
                std::cout << e.getName() << " retreats to (" << newX << ", " << newY << ")\n";
                retreated = true;
                break;
            }
        }

        if (!retreated) {
            std::cout << e.getName() << " tries to retreat but finds no space!\n";
        }
        return;
    }

    // Step 2: Check how many abilities in rangeCheck if a target is in range
    const auto& abilities = characterAbilities[&e];
    bool targetInRange = false;
    int abilitiesWithinRange = 0;
    for (const auto& a : abilities) {
        if (e.getCurrentMana() < a.cost) continue;
        if (a.abilityTarget != "self" && isInRange(e, *target, a.range)) {
            targetInRange = true;
            abilitiesWithinRange++;
            break;
        }
    }

    // Step 2: chance to move toward target if out of range
    bool moved = false; 
    if (abilitiesWithinRange < 2) {
        // 1:6 chance to use a self targetting ability
        if(RPGUtils::rollDie(5) == 1) {
            // do not move
        } else {
            Position myPos = e.getPosition();
            Position tPos = target->getPosition();

            int dx = (tPos.x > myPos.x) ? 1 : (tPos.x < myPos.x) ? -1 : 0;
            int dy = (tPos.y > myPos.y) ? 1 : (tPos.y < myPos.y) ? -1 : 0;

            std::vector<std::pair<int, int>> directions = {
                {dx, dy}, {dx, 0}, {0, dy}, {0, 0}
            };

            for (auto [mx, my] : directions) {
                int newX = myPos.x + mx;
                int newY = myPos.y + my;

                if (newX >= 0 && newX < GRID_WIDTH &&
                    newY >= 0 && newY < GRID_HEIGHT &&
                    combatGrid[newY][newX] == nullptr) {
                    combatGrid[myPos.y][myPos.x] = nullptr;
                    combatGrid[newY][newX] = &e;
                    e.setPosition(newX, newY);
                    std::cout << e.getName() << " moves to (" << newX << ", " << newY << ")\n";
                    moved = true;
                    break;
                }
            }
        }
    }

    // Step 3: Try attacking if didn’t move or already in range
    if (!moved) {

        const Ability* chosen = nullptr;
        // Choose an ability in range
        for (const auto& a : abilities) {
            if (e.getCurrentMana() >= a.cost &&
                (a.abilityTarget == "self" || isInRange(e, *target, a.range))) {
                chosen = &a;
                break;
            }
        }

        if (chosen) {
            useAbility(e, target);
        } else {
            std::cout << e.getName() << " ends turn (no usable ability in range).\n";
        }
    }
}

int combatManager::manhattanDistance(const Position& a, const Position& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

Character* combatManager::chooseAITarget(const Character& actor, AIDifficulty difficulty) {
    Character* lowestHP = nullptr;
    Character* firstSummoned = nullptr;
    int minHP = INT_MAX;

    // possible target vector
    std::vector<Character*> possibleTarget;
    possibleTarget.clear();

    // Iterate through alive combatants
    for (Character* c : allCombatants) {
        if (!c->isAlive() || c == &actor)
            continue;

        // Only choose from opposite team
        if (c->getTeam() != actor.getTeam()) {
            possibleTarget.push_back(c);
        }
    }

    if (possibleTarget.empty()) {
        std::cout << "There are no valid targets.\n";
        return nullptr;
    }

    // Easy: Random
    if (difficulty == AIDifficulty::Easy) {
        return possibleTarget[RPGUtils::rollDice(1, possibleTarget.size()) - 1];
    }

    // Medium: Pick lowest HP
    if (difficulty == AIDifficulty::Medium) {
        return *std::min_element(possibleTarget.begin(), possibleTarget.end(),
            [](Character* a, Character* b) {
                return a->getCurrentHealth() < b->getCurrentHealth();
            });
    }

    // Hard: Prioritize high-threat targets (Human > Summoned > NPC), then lowest HP
    Character* bestTarget = nullptr;
    float bestScore = -1.0f;
    for (Character* t : possibleTarget) {
        float threatScore = 0.0f;

        if (t->getController() == ControllerType::Human) threatScore += 3.0f;
        else if (t->getController() == ControllerType::Summoned) threatScore += 2.0f;
        else if (t->getController() == ControllerType::NPC) threatScore += 1.0f;

        float hpRatio = static_cast<float>(t->getCurrentHealth()) / t->getMaxHealth();
        threatScore += (1.0f - hpRatio); // lower HP → higher priority

        if (threatScore > bestScore) {
            bestScore = threatScore;
            bestTarget = t;
        }
    }

    return bestTarget;
}

void combatManager::resolveAbility(Character& user, Character& target, const core::Ability& a) {
    std::cout << user.getName() << " uses " << a.name << " on " << target.getName() << std::endl;
    
    //Spend Mana
    user.setHealthMana("M", -a.cost);

    // 1) Roll to hit
    int toHit = RPGUtils::rollDice(a.hitRoll.count, a.hitRoll.sides)
                + a.hitRoll.bonus;

    std::this_thread::sleep_for(std::chrono::seconds(1));
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

void combatManager::useAbility(Character& c, Character* aiTarget) {
    bool isAI = c.getController() != ControllerType::Human;

    const auto& usableAbilities = characterAbilities[&c];
    if (usableAbilities.empty()) {
        std::cout << c.getName() << " has no abilities to use.\n";
        return;
    }

    const Ability* chosen = nullptr;

    if (isAI) {
        int idx = RPGUtils::rollDice(1, usableAbilities.size()) - 1;
        chosen = &usableAbilities[idx];
    } else {
        std::cout << "\nAvailable Abilities:\n";
        for (size_t i = 0; i < usableAbilities.size(); ++i) {
            const auto& a = usableAbilities[i];
            std::cout << "[" << i << "] " << a.name
                      << " (" << a.cost << " mana)"
                      << (a.abilityTarget == "self" ? " (self target)" : "") << "\n";
        }
    std::cout << ">";
        size_t abilityChoice;
        std::cin >> abilityChoice;
        chosen = &usableAbilities[abilityChoice];
    }

    if (c.getCurrentMana() < chosen->cost) {
        std::cout << c.getName() << " does not have enough mana for " << chosen->name << ".\n";
        return;
    }

    // Self-targeting
    if (chosen->abilityTarget == "self") {
        resolveAbility(c, c, *chosen);
        return;
    }

    // Build valid targets
    std::vector<Character*> possibleTarget;
    for (Character* target : allCombatants) {
        if (!target->isAlive()) continue;
        if (target == &c) continue;
        if (target->getTeam() == c.getTeam()) continue;
        possibleTarget.push_back(target);
    }

    if (possibleTarget.empty()) {
        std::cout << "No valid targets available.\n";
        return;
    }

    Character* target = nullptr;
    if (isAI) {
        target = aiTarget;
    } else {
        std::cout << "Available targets:\n";
        for (size_t i = 0; i < possibleTarget.size(); ++i) {
            std::cout << "[" << i << "] " << possibleTarget[i]->getName()
                      << " - HP: " << possibleTarget[i]->getCurrentHealth() << "/"
                      << possibleTarget[i]->getMaxHealth() << "\n";
        }

        std::cout << "Choose a Target > ";
        size_t targetChoice;
        std::cin >> targetChoice;
        target = possibleTarget[targetChoice];
    }

    // Range check
    if (!isInRange(c, *target, chosen->range)) {
        std::cout << chosen->name << " is out of range! The ability fails.\n";
        c.setHealthMana("M", -chosen->cost);
        return;
    }

    resolveAbility(c, *target, *chosen);
}

void combatManager::moveCharacter(Character& c, int dx, int dy) {
    Position pos = c.getPosition();
    int newX = pos.x + dx;
    int newY = pos.y + dy;

    // Bounds and collision check
    if (newX < 0 || newX >= GRID_WIDTH || newY < 0 || newY >= GRID_HEIGHT) {
        std::cout << "Cannot move outside grid!\n";
        return;
    }
    if (combatGrid[newY][newX] != nullptr) {
        std::cout << "Tile is occupied!\n";
        return;
    }

    // Move
    combatGrid[pos.y][pos.x] = nullptr;
    combatGrid[newY][newX] = &c;
    c.setPosition(newX, newY);
    std::cout << c.getName() << " moved to (" << newX << ", " << newY << ")\n";
}

void combatManager::meditate(Character& c) {   
    int gainHealth = std::ceil(c.getMaxHealth() * MEDITATE_HEALTH);
    int gainMana   = std::ceil(c.getMaxMana()   * MEDITATE_MANA);

    // Clamp to not exceed max
    int newHealth = std::min(c.getCurrentHealth() + gainHealth, c.getMaxHealth());
    int newMana   = std::min(c.getCurrentMana()   + gainMana,   c.getMaxMana());

    // Compute actual gains (in case clamped)
    int actualGainHealth = newHealth - c.getCurrentHealth();
    int actualGainMana   = newMana   - c.getCurrentMana();

    std::cout << c.getDisplayName() << " meditates ";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "+" << actualGainHealth << " HP\n";
    std::cout << "+" << actualGainMana   << " MP\n\n";

    // Set new clamped values
    c.setCurrentHealth(newHealth);
    c.setCurrentMana(newMana);

    c.printCombatStats();
}

bool combatManager::isInRange(const Character& user, const Character& target, int range) {
    // Get position of user and target
    Position userPosition = user.getPosition();
    Position targetPosition = target.getPosition();
    
    // See if user and target are on the same x or y line.
    // For example if user on (x,y)=(1,0) and target on on (x,y)=(1,3), they are on the same row...
    int dx = std::abs(userPosition.x - targetPosition.x);
    int dy = std::abs(userPosition.y - targetPosition.y);

    // std::cout << "[DEBUG] User: (" << userPosition.x << ", " << userPosition.y << ")\n";
    // std::cout << "[DEBUG] Target: (" << targetPosition.x << ", " << targetPosition.y << ")\n";
    // std::cout << "[DEBUG] dx = " << dx << ", dy = " << dy << "\n";

    if (dx == 0 || dy == 0) {
        // Cardinal direction (straight)
        return std::max(dx, dy) <= range;
    } else if (dx == dy) {
        // Perfect diagonal
        if (range < 3) {
            return dx <= (range);
        } else {
            return dx <= (range - 1);
        }
    }
    // Not a straight or diagonal line — disallow
    return false;
}

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

std::vector<std::unique_ptr<Character>> combatManager::loadCompanionParty(const Character& companionLeader) {
    std::vector<std::unique_ptr<Character>> companions;

    if (!companionLeader.isCompanion()) return companions;

    for (const auto& id : companionLeader.getCompanionList()) {
        std::string filename = "assets/mobs/" + id + ".json";
        auto c = loadMonster(filename);  // your existing loader
        if (c) companions.push_back(std::move(c));
        else std::cerr << "[ERROR] Failed to load companion: " << filename << "\n";
    }

    return companions;
}