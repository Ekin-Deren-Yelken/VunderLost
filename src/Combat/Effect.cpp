#include "../../include/Effect.h"

using core::Effect;
using core::DamageType;
using core::StatusType;

// e.g. create a fireball effect:
Effect e1{ DamageType::Fire, 12 };
Effect e2{ StatusType::Scorched, 3, /*dot=*/4 };