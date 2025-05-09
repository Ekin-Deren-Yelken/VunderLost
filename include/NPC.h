#pragma once

#include "Character.h"

class NPC : public Character {
public:
    NPC::NPC() : favour(0) {}

    void setFavour(int value);
    int getFavour() const;
    void adjustFavour(int delta);
    bool isFriendly() const;

    void loadFromJson(const nlohmann::json& j) override;

private:
    int favour = 0;
};
