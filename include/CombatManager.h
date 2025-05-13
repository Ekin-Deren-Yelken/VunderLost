#pragma once

#include <vector>
#include <unordered_map> 
#include "Ability.h"
#include "character.h"
#include "AIDifficulty.h"

namespace combat {

class combatManager {
public:
    /**
     * Starts a turn-based encounter between player characters and enemies.
     * @param players Vector of pointers to player-controlled Characters
     * @param enemies Vector of pointers to enemy Characters
     */
    bool startEncounter(std::vector<Character*>& enemies, std::vector<Character*>& players);

    std::unique_ptr<Character> loadMonster(const std::string& relPath);
    
private:
    std::vector<Character*>      allCombatants;   // combined turn order
    std::map<Character*, std::vector<core::Ability>> characterAbilities;       // list of available abilities by character
    size_t                       activeIndex = 0; // whose turn it is

    // Grid Dimensions
    static constexpr int GRID_WIDTH = 6;
    static constexpr int GRID_HEIGHT = 6;

    std::vector<std::vector<Character*>> combatGrid;

    // Meditate Controls
    static constexpr float MEDITATE_HEALTH = 0.05;
    static constexpr float MEDITATE_MANA = 0.08;

    // Loads a prototype list of abilities (hard-coded or from JSON)
    void loadAbilities();

    // Initialize combat grid
    void initGrid(std::vector<Character*>& players, std::vector<Character*>& enemies);\
    void printCombatGrid() const;

    // Advances to the next turn in the encounter
    void nextTurn();
    // Handles a human player's turn
    void playerTurn(Character& p);
    // Handles an AI-controlled enemy's turn
    void enemyTurn(Character& e);

    int manhattanDistance(const Position& a, const Position& b);

    // AI smart Targetting
    Character* chooseAITarget(const Character& actor, AIDifficulty difficulty);

    void summondTurn(Character& e);

    // Turn choices
    void useAbility(Character& c, Character* aiTarget = nullptr);
    void moveCharacter(Character& c, int dx, int dy);
    void meditate(Character& c);
    bool isInRange(const Character& user, const Character& target, int range);

    /**
     * Applies an ability from user to target:
     *  - spends resources
     *  - accuracy check (and QTE hook)
     *  - applies each Effect via Character::applyEffect()
     */
    void resolveAbility(Character& user,
                        Character& target,
                        const core::Ability& a);

    // Applies end-of-turn effects (DoT, status decay)
    void endTurnCleanup();

    std::vector<std::unique_ptr<Character>> loadCompanionParty(const Character& companionLeader);
};

} // namespace combat
