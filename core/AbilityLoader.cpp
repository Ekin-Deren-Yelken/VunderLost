#include "AbilityLoader.h"
#include "status_util.h"
#include "../0-utils/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace core {

    const std::unordered_map<std::string, Ability>& getAllAbilities() {
        static auto map = [](){
            // Check try to open abilities.json
            std::ifstream in("5-Combat/data/abilities.json");
            if (!in.is_open()) {
                std::cerr << "Failed to open abilities.json at: "
                          << std::filesystem::current_path() / "data/abilities.json" << "\n";
                std::exit(1);
            }
            
            // Check if abilities.json is empty
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size == 0) {
                std::cerr << "abilities.json is empty!\n";
                std::exit(1);
            }
            in.seekg(0);
            
            // Parse abilities.json
            json arr;
            try {
                in >> arr;
            } catch (const json::parse_error& e) {
                std::cerr << "Failed to parse abilities.json: " << e.what() << std::endl;
                std::exit(1);
            }

            std::unordered_map<std::string, Ability> m;

            for (const auto& j : arr) {
                Ability a;
                a.name         = j.at("name").get<std::string>();
                a.cost         = j.at("cost").get<int>();
                a.hitThreshold = j.at("hitThreshold").get<int>();
                a.range        = j.at("range").get<int>();
                a.requiresQTE  = j.at("requiresQTE").get<bool>();
                a.qteWindowMs  = j.at("qteWindowMs").get<int>();
                a.profession   = j.at("profession").get<std::string>();
    
                // Optional comment
                if (j.contains("comment"))
                    a.comment = j.at("comment").get<std::string>();
    
                // Hit roll
                const auto& hr = j.at("hitRoll");
                a.hitRoll = { hr.at("count"), hr.at("sides"), hr.at("bonus") };
    
                // Damage roll
                const auto& dr = j.at("damageRoll");
                a.damageRoll = { dr.at("count"), dr.at("sides"), dr.at("bonus") };
    
                // Effects
                for (auto& ef : j.at("effects")) {
                    auto st   = stringToStatusType(ef.at("status").get<std::string>());
                    int  dur  = ef.at("duration").get<int>();
                    int  dot  = ef.value("dotValue", 0);
                    int  dmg  = ef.value("dmgValue", 0);  // If any instant damage
                    auto dt   = stringToDamageType(ef.value("damageType", "Physical"));
                
                    a.effects.emplace_back(st, dur, dot, dmg, dt); 
                }
    
                m.emplace(j.at("id").get<std::string>(), std::move(a));
            }
    
            return m;
        }();
    
        return map;
    }
}