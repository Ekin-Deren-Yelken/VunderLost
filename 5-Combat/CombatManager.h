#pragma once

#include <vector>
#include <unordered_map> 
#include "../core/Ability.h"
#include "../1-Characters/character.h"

namespace combat {

class CombatManager {
public:
    /**
     * Starts a turn-based encounter between player characters and enemies.
     * @param players Vector of pointers to player-controlled Characters
     * @param enemies Vector of pointers to enemy Characters
     */
    void startEncounter(std::vector<Character*>& players,
                        std::vector<Character*>& enemies);

private:
    std::vector<Character*>      allCombatants;   // combined turn order
    std::map<Character*, std::vector<Ability>> characterAbilities;       // list of available abilities by character
    size_t                       activeIndex = 0; // whose turn it is

    // Loads a prototype list of abilities (hard-coded or from JSON)
    void loadAbilities();

    // Advances to the next turn in the encounter
    void nextTurn();

    // Handles a human player's turn
    void playerTurn(Character& p);

    // Handles an AI-controlled enemy's turn
    void enemyTurn(Character& e);

    Character* CombatManager::chooseAITarget();

    void summondTurn(Character& e);

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
};

} // namespace combat
