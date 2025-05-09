#include "stat_utils.h"
#include "../core/Effect.h"
#include "../core/status_util.h"
#include "../core/AbilityLoader.h"
#include <iostream>
#include "character.h"

// ─── Constructors ────────────────────────────────────────────────────────────
// Default Constructor
Character::Character()
    : name("Unknown")
    , currentHealth(0)
    , maxHealth(0)
    , currentMana(0)
    , maxMana(0)
    , armour(0)
    , XP(0)
    , level(1)
    , trainingPoints(0)
    , act(0)
    , controller(core::ControllerType::AI)
{
    stats = {{"STR",0}, {"INT",0}, {"DEX",0}, {"GIRTH",0}, {"LUCK",0}, {"CHARM",0}};
}

Character::Character(const std::string&  name,
                     const std::string&  race,
                     const std::string&  profession,
                     const std::string&  sex,
                     const std::string&  gender,
                     int                 armour,
                     int                 XP,
                     int                 level,
                     int                 act)
    : name(name), race(race), profession(profession),
      sex(sex), gender(gender),
      armour(armour), XP(XP), level(level), trainingPoints(0), act(act), controller(core::ControllerType::AI)
{
    stats = {{"STR",0},{"INT",0},{"DEX",0},{"GIRTH",0},{"LUCK",0},{"CHARM",0}};
    applyStatModifiers(stats, race, sex, gender, profession);
    calculateVitals();
}

void Character::calculateVitals() {
    maxHealth = 50 + getStat("STR") * 10;
    maxMana = 30 + getStat("INT") * 8;
}

// ─── Controller API ─────────────────────────────────────────────────────────
void Character::setController(core::ControllerType c) { controller = c; }
core::ControllerType Character::getController() const { return controller; }
bool Character::isPlayer() const { return controller == core::ControllerType::Human; }

// ─── Stat and Attribute Setters ──────────────────────────────────────────────
void Character::setStat(const std::string& statName, int value) { stats[statName] = value; }
void Character::setHealthMana(std::string type, int delta) {
    if (type == "H") setHealth(delta);
    else if (type == "M") setMana(delta);
}
void Character::setRace(const std::string& raceValue) { race = raceValue; }
void Character::setSex(const std::string& sexValue) { sex = sexValue; }
void Character::setGender(const std::string& genderValue) { gender = genderValue; }
void Character::setAct(int value) { act = value; }
void Character::setProfession(const std::string& professionValue) { profession = professionValue; }

void Character::setHealth(int delta) { currentHealth = std::clamp(currentHealth + delta, 0, maxHealth); }
void Character::setMana(int delta) { currentMana = std::clamp(currentMana + delta, 0, maxMana); }

void Character::setCurrentHealth(int health) { currentHealth = health; }
void Character::setCurrentMana(int mana) { currentMana = mana; }

// ─── Status Checks ───────────────────────────────────────────────────────────
bool Character::isAlive() const { return currentHealth > 0; }

// ─── Stat and Attribute Getters ──────────────────────────────────────────────
int         Character::getStat(const std::string& statName) const {
    auto it = stats.find(statName);
    if (it != stats.end())
        return it->second;
    return 0;
}
void Character::recalculateStats(const std::string& race, const std::string& sex, const std::string& gender, const std::string& profession) {
    auto updatedStats = applyStatModifiers(stats, race, sex, gender, profession);

    for (const auto& [key, value] : updatedStats) {
        std::cout << "[DEBUG] " << key << " before: " << getStat(key) << ", after: " << value << "\n";
        setStat(key, value);
    }

    calculateVitals(); // recalc max HP/mana
    setHealth(getMaxHealth());
    setMana(getMaxMana());
}
std::string Character::getName() const { return name; }
std::string Character::getRace() const { return race.value_or("Unset"); }
std::string Character::getSex() const { return sex.value_or("Unset"); }
std::string Character::getGender() const { return gender.value_or("Unset"); }
std::string Character::getProfession() const { return profession.value_or("Unset"); }
int         Character::getAct() const { return act; }
int         Character::getXP() const { return XP; }
int         Character::getLevel() const { return level;}
int         Character::getTrainingPoints() const { return trainingPoints; }

int         Character::getMaxHealth() const { return maxHealth; }
int         Character::getMaxMana() const { return maxMana; }
int         Character::getCurrentHealth() const { return currentHealth; }
int         Character::getCurrentMana() const { return currentMana; }

// ─── XP and Leveling ─────────────────────────────────────────────────────────
void Character::spendTrainingPoint(const std::string& statName) {
    if (trainingPoints <= 0) {
        std::cout << "No training points available.\n";
        return;
    }

    if (stats.find(statName) != stats.end()) {
        stats[statName] += 1;
        trainingPoints--;
        std::cout << statName << " increased to " << stats[statName] << ". Training points left: " << trainingPoints << "\n";
    } else {
        std::cout << "Invalid stat name.\n";
    }
}
void Character::increaseXP(int amount) {
    XP += amount;
    std::cout << name << " gains " << amount << " XP!\n";

    while (XP >= 100) {
        XP -= 100;
        level++;
        levelUp();
    }
}
void Character::levelUp() {
    level++;
    std::cout << name << " leveled up to level " << level << "\n";

    // Show current stats
    std::cout << "\nCurrent stats:\n";
    for (const auto& stat : stats) {
        std::cout << stat.first << ": " << stat.second << "\n";
    }

    std::string choice;
    while (true) {
        std::cout << "\nChoose a stat to increase (STR, DEX, INT, GIRTH, LUCK, CHARM), or choose SKIP to save your training point to your next visit to the Tavern";
        std::cin >> choice;

        // Convert input to uppercase
        for (auto& c : choice) {
            c = toupper(c);
        }

        if (choice == "SKIP") {
            trainingPoints += 1;
            std::cout << "You saved a training point. Total training points: " << trainingPoints << "\n";
            break;
        }

        if (stats.find(choice) != stats.end()) {
            stats[choice] += 1;
            std::cout << choice << " increased to " << stats[choice] << "!\n";
            break;
        } else {
            std::cout << "Invalid stat. Try again.\n";
        }
    }
}

// ───  Display Stats ──────────────────────────────────────────────────────────
void Character::printStats() const {
    std::cout << "Health: " << currentHealth << "/" << maxHealth << "\n"
              << "Mana: " << currentMana << "/" << maxMana << "\n"
              << "Armor: " << armour << "\n"
              << "XP: " << XP << "\n"
              << "Level: " << level << "\n"
              << "Act: " << act << "\n";


    for (const auto& stat : stats) {
        std::cout << stat.first << ": " << stat.second << "\n";
    }
}

void Character::printCombatStats() const {
    std::cout << "\nHealth: " << currentHealth << "/" << maxHealth << "\n"
              << "Mana: " << currentMana << "/" << maxMana << "\n";
}

// ─── Titles ───────────────────────────────────────────────────────────────────
void Character::addTitle(const std::string& title) {
    titles.push_back(title);
    currentTitle = title;
}
std::string Character::getDisplayName() const {
    return !currentTitle.empty() ? name + ", " + currentTitle : name;
}

// ─── Serialization ────────────────────────────────────────────────────────────
json Character::toJson() const {
    return {
        {"name", name},
        {"level", level},
        {"XP", XP},
        {"armor", armour},
        {"maxHealth", maxHealth},
        {"currentHealth", currentHealth},
        {"maxMana", maxMana},
        {"currentMana", currentMana},
        {"trainingPoints", trainingPoints},
        {"currentTitle", currentTitle},
        {"stats", stats},
        {"titles", titles},
        {"race", race.value_or("")},
        {"sex", sex.value_or("")},
        {"gender", gender.value_or("")},
        {"act", act},
        {"profession", profession.value_or("")},
        {"controller", static_cast<int>(controller)}
    };
}

void Character::loadFromJson(const json& j) {
    name = j.at("name");
    level = j.at("level");
    XP = j.at("XP");
    armour = j.at("armor");
    
    maxHealth = j.at("maxHealth");
    currentHealth = j.at("currentHealth");

    maxMana = j.at("maxMana");
    currentMana = j.at("currentMana");

    trainingPoints = j.at("trainingPoints");
    currentTitle = j.at("currentTitle");
    stats = j.at("stats").get<std::map<std::string, int>>();
    titles = j.at("titles").get<std::vector<std::string>>();
    
    race = j.at("race").get<std::string>();
    sex = j.at("sex").get<std::string>();
    gender = j.at("gender").get<std::string>();
    act = j.at("act");
    if (j.contains("profession")) {
        profession = j.at("profession").get<std::string>();
    }
    if (j.contains("controller")) {
        int c = j.at("controller").get<int>();
        controller = static_cast<core::ControllerType>(c);
    }
    if (j.contains("abilities")) {
        auto const& dict = core::getAllAbilities();
        for (auto& idj : j.at("abilities")) {
          auto id = idj.get<std::string>();
          auto it = dict.find(id);
          if (it != dict.end())
            addAbility(it->second);
          else
            std::cerr << "Unknown ability ID: " << id << "\n";
        }
      }
}

// ─── Combat Hooks ─────────────────────────────────────────────────────────────
void Character::applyEffect(const core::Effect& e, const std::string& sourceName) {
    // --- Instant Damage with multipliers from active statuses ---

    // Only handle effects that deal damage in this method
    if (e.dmgValue > 0) {
        // Calculate effective damage, apply modifier.
        float dmg = e.dmgValue;
        for (auto& node : activeStatuses) {
            dmg *= node.dmgMultiplier;
        }
        int finalDmg = static_cast<int>(dmg);
        currentHealth -= finalDmg;

        std::cout << name << " takes " << finalDmg << " "
          << core::damage_toString(e.dmgType)
          << " damage from " << sourceName << ".\n";

    }

    // --- Add new status effect to the list ---
    if (e.status != core::StatusType::None) {
        StatusNode node{};
        node.status    = e.status;
        node.turnsLeft = e.duration;
        // Only Bleed, Scorched, Poisoned, Frostbitten deal DoT
        node.dotValue  = (e.status == core::StatusType::Bleed
                       || e.status == core::StatusType::Scorched
                       || e.status == core::StatusType::Poisoned
                       || e.status == core::StatusType::Frostbitten)
                         ? e.dotValue
                         : 0;
        node.dmgMultiplier    = 1.0f;
        node.accuracyModifier = 1.0f;
        node.flatDamageBuff   = 0;
        node.damageType = e.dmgType;

        switch (e.status) {
            case core::StatusType::Shocked:
                // Shocked: reduce accuracy by 25% per dotValue
                node.accuracyModifier = std::max(0.0f, 1.0f - e.dmgValue * 0.25f);
                break;
            case core::StatusType::Fortify:
                // Fortify: reduce incoming damage by 10% per dotValue
                node.dmgMultiplier = std::max(0.0f, 1.0f - e.dmgValue * 0.10f);
                break;
            case core::StatusType::DMG:
                // DMG: increase incoming damage by 10% per dotValue
                node.dmgMultiplier = 1.0f + e.dmgValue * 0.10f;
                break;
            default:
                break;
        }

        activeStatuses.push_back(node);
        std::cout << name
                  << " gains " << core::status_toString(node.status)
                  << " for " << e.duration << " turns.\n";
    }
}

void Character::tickStatuses() {
    for (auto& node : activeStatuses) {
        // --- Damage over Time ---
        if (node.dotValue > 0) {
            setHealth(-node.dotValue);
            std::cout << name << " takes " << node.dotValue
                      << " " << core::damage_toString(node.damageType) << " damage from "
                      << core::status_toString(node.status)
                      << " (" << node.turnsLeft << " turns left)\n";
            std::cout << name << ": ";
            printCombatStats();
        }
        // Decrement duration
        --node.turnsLeft;
    }
    // Remove expired statuses
    activeStatuses.erase(
        std::remove_if(
            activeStatuses.begin(),
            activeStatuses.end(),
            [](const StatusNode& n) { return n.turnsLeft <= 0; }
        ),
        activeStatuses.end()
    );
}
void Character::applyInstantDamage(int dmg, core::DamageType type, const std::string& source, bool suppressPrint) {
    float finalDmg = static_cast<float>(dmg);
    for (const auto& node : activeStatuses)
        finalDmg *= node.dmgMultiplier;

    int finalInt = static_cast<int>(finalDmg);
    currentHealth -= finalInt;

    if (!suppressPrint) {
        std::cout << name << " takes " << finalDmg
                  << " " << core::damage_toString(type)
                  << " damage from " << source << ".\n";
    }
}


// ─── Ability API ─────────────────────────────────────────────────────────────
void Character::addAbility(const core::Ability& a) {
    abilities.push_back(a);
}

const std::vector<core::Ability>& Character::getAbilities() const {
    return abilities;
}
