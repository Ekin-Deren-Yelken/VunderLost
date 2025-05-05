#pragma once
#include <string>
#include <unordered_map>
#include "Ability.h"

namespace core {
  // Reads the JSON once and returns a map of all abilities by id
  const std::unordered_map<std::string, Ability>& getAllAbilities();
}
