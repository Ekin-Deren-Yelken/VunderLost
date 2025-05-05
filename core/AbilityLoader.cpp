#include "AbilityLoader.h"
#include "status_util.h"
#include "../0-utils/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace core {
    const std::unordered_map<std::string, Ability>& getAllAbilities() {
        static auto map = [](){
            std::ifstream in("5-Combat/data/abilities.json");
            if (!in.is_open()) {
                std::cerr << "Failed to open abilities.json at: "
                          << std::filesystem::current_path() / "data/abilities.json" << "\n";
                std::exit(1);
            }
    
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size == 0) {
                std::cerr << "abilities.json is empty!\n";
                std::exit(1);
            }
            in.seekg(0);
    
            json arr;
            try {
                in >> arr;
            } catch (const json::parse_error& e) {
                std::cerr << "Failed to parse abilities.json: " << e.what() << std::endl;
                std::exit(1);
            }
    
            std::unordered_map<std::string, Ability> m;
            for (auto& j : arr) {
                Ability a;
                auto id = j.at("id").get<std::string>();
                a.name         = j.at("name").get<std::string>();
                a.cost         = j.at("cost").get<int>();
                a.profession   = j.at("profession").get<std::string>();
                auto hr        = j.at("hitRoll");
                a.hitRoll      = { hr.at("count"), hr.at("sides"), hr.at("bonus") };
                a.hitThreshold = j.at("hitThreshold").get<int>();
                auto dr        = j.at("damageRoll");
                a.damageRoll   = { dr.at("count"), dr.at("sides"), dr.at("bonus") };
                a.range        = j.at("range").get<int>();
                a.requiresQTE  = j.at("requiresQTE").get<bool>();
                a.qteWindowMs  = j.at("qteWindowMs").get<int>();
                if (j.contains("comment"))
                    a.comment  = j.at("comment").get<std::string>();
                for (auto& ef : j.at("effects")) {
                    auto st    = stringToStatusType(ef.at("status").get<std::string>());
                    int dur    = ef.at("duration").get<int>();
                    int dot    = ef.value("dotValue", 0);
                    a.effects.emplace_back(st, dur, dot);
                }
                m.emplace(std::move(id), std::move(a));
            }
            return m;
        }();
        return map;
    }
}