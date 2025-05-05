#pragma once
#include <string>
#include "Effect.h"  // for StatusType

namespace core {
  inline StatusType stringToStatusType(const std::string& s) {
    if      (s == "Bleed")       return StatusType::Bleed;
    else if (s == "Scorched")    return StatusType::Scorched;
    else if (s == "Shocked")     return StatusType::Shocked;
    else if (s == "Frostbitten") return StatusType::Frostbitten;
    else if (s == "Poisoned")    return StatusType::Poisoned;
    else if (s == "Fortified")   return StatusType::Fortify;
    else if (s == "DMG")         return StatusType::DMG;
    else                          return StatusType::None;
  }
}
