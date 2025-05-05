#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <map>
#include <vector>
#include <optional>
#include "../0-utils/json.hpp"
#include "../core/Effect.h"
#include "../core/Ability.h"

using json = nlohmann::json;

namespace core {

    enum class ControllerType {
        AI,           // fully automated
        Human,        // needs player input
        Summoned      // “belongs” to another Character
    };
}
class Character {

protected:
    std::string                 name;
    std::vector<std::string>    titles;
    std::string                 currentTitle;
    std::optional<std::string>  race;
    std::optional<std::string>  profession;
    std::optional<std::string>  sex;
    std::optional<std::string>  gender;
    
    int                         maxHealth;
    int                         maxMana;
    int                         currentHealth;
    int                         currentMana;
    int                         armour;
    int                         XP;
    int                         level;
    int                         trainingPoints;
    int                         act;
    std::map<std::string, int>  stats; // create a map called stats where a string is linked to some int

    core::ControllerType controller = core::ControllerType::AI; // player, enemy, or summonded/minion turn

    // Active status effects (damage-over-time or debuffs)
    // simple, numeric tweaks to combat
    /*
    - On tickStatuses(), only handle DoT via dotValue (damage)
    - Damage calculation > multiply by dmgMultiplier.
    - Hit‐roll > multiply base accuracy by accuracyModifier.
    - If buffFlatDamage>0 > add that into your next applyEffect() call and then zero it out.
    */
    // Make it easier to create new effects, ie Frozen = accuracyModifier = 0
    struct StatusNode {
        core::StatusType status; // type of status effect
        int              turnsLeft; // turns remaining
        int              dotValue; // damage-per-turn (0 if none)
        float            dmgMultiplier; // multiplies incoming damage (e.g. 1.2 for +20%)
        float            accuracyModifier; // modifies hit chance (e.g. 0.8 for -20%)
        int              flatDamageBuff; // flat bonus added to next outgoing damage
        core::DamageType damageType = core::DamageType::Physical;
    };
    std::vector<StatusNode> activeStatuses;

    // Abilities known by this character
    std::vector<core::Ability> abilities;

public:
    Character();  // Default constructor

    Character(const std::string& name, 
              const std::string& race, 
              const std::string& profession, 
              const std::string& sex, 
              const std::string& gender, 
              int armour, 
              int XP, 
              int level, 
              int act);
    
    // Controller API
    void setController(core::ControllerType c);
    core::ControllerType getController() const;
    bool isPlayer() const;
    
    // Stat and attribute setters
    void setStat(const std::string& statName, int val);
    void setHealthMana(std::string type, int delta);
    void setRace(const std::string& raceValue);
    void setProfession(const std::string& professionValue);
    void setSex(const std::string& sexValue);
    void setGender(const std::string& genderValue);
    void setAct(int value);
    void addTitle(const std::string& title);

    void setHealth(int delta);
    void setMana(int delta);

    //  Sttus Checks
    bool isAlive() const;

    // Stat and attribute getters
    int getStat(const std::string& statName) const;
    std::string getName() const;
    std::string getRace() const;
    std::string getSex() const;
    std::string getGender() const;
    std::string getProfession() const;
    int getAct() const;
    int getXP() const;
    int getLevel() const;
    int getTrainingPoints() const;
    std::string getDisplayName() const;
    
    int getMaxHealth() const;
    int getMaxMana() const;
    int getCurrentHealth() const;
    int getCurrentMana() const;
    
    // XP and Leveling
    void spendTrainingPoint(const std::string& statName);
    void increaseXP(int amount);
    void levelUp();
    
    // Calculations and display
    void calculateVitals();
    void printStats() const;
    void printCombatStats() const;
    
    // Serialization
    json toJson() const;
    void loadFromJson(const json& j);

    // Combat hooks
    void applyEffect(const core::Effect& e, const std::string& sourceName);
    void tickStatuses();

    // Ability API
    void addAbility(const core::Ability& a);
    const std::vector<core::Ability>& getAbilities() const;

};

enum class Stat {
    STR,
    DEX,
    INT,
    LUCK,
    CHARM,
    GIRTH
};


#endif