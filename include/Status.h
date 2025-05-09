#pragma once

#include "Effect.h"
#include <string>

namespace core {
    std::string status_toString(StatusType type);
    std::string damage_toString(DamageType type);
    StatusType stringToStatusType(const std::string& s);
    DamageType stringToDamageType(const std::string& s);
}
