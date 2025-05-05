#include "status_util.h"
#include <unordered_map>

namespace core {

    std::string status_toString(StatusType type) {
        switch (type) {
            case StatusType::None:        return "None";
            case StatusType::Bleed:       return "Bleed";
            case StatusType::Scorched:    return "Scorched";
            case StatusType::Shocked:     return "Shocked";
            case StatusType::Frostbitten: return "Frostbitten";
            case StatusType::Poisoned:    return "Poisoned";
            case StatusType::Fortify:     return "Fortify";
            case StatusType::DMG:         return "Vulnerable";
            default:                      return "Unknown";
        }
    }

    std::string damage_toString(DamageType type) {
        switch (type) {
            case DamageType::Physical:  return "Physical";
            case DamageType::Fire:      return "Fire";
            case DamageType::Lightning: return "Lightning";
            case DamageType::Frost:     return "Frost";
            case DamageType::Poison:    return "Poison";
            default:                    return "Unknown";
        }
    }

    StatusType stringToStatusType(const std::string& s) {
        if (s == "None")        return StatusType::None;
        if (s == "Bleed")       return StatusType::Bleed;
        if (s == "Scorched")    return StatusType::Scorched;
        if (s == "Shocked")     return StatusType::Shocked;
        if (s == "Frostbitten") return StatusType::Frostbitten;
        if (s == "Poisoned")    return StatusType::Poisoned;
        if (s == "Fortify")     return StatusType::Fortify;
        if (s == "DMG")         return StatusType::DMG;
        return StatusType::None;
    }

    DamageType stringToDamageType(const std::string& s) {
        if (s == "Fire")      return DamageType::Fire;
        if (s == "Frost")     return DamageType::Frost;
        if (s == "Poison")    return DamageType::Poison;
        if (s == "Lightning") return DamageType::Lightning;
        return DamageType::Physical;
    }

}