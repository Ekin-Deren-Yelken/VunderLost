#include <cstdlib>
#include <string>
#include <stdexcept>
#include "../../include/apikey_loader.h"

std::string getOpenAIKey() {
    const char* key = std::getenv("OPENAI_API_KEY");
    if (!key) {
        throw std::runtime_error("Environment variable OPENAI_API_KEY not set.");
    }
    return std::string(key);
}