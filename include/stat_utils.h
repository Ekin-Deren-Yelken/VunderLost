#pragma once
#include <string>
#include <map>

std::map<std::string, int> getBaseStats();

std::map<std::string, int> applyStatModifiers(
    std::map<std::string, int> stats,
    const std::string& race,
    const std::string& sex,
    const std::string& gender,
    const std::string& profession
);

