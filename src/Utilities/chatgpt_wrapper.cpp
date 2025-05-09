#include "chatgpt_wrapper.h"

#include <iostream>
#include <curl/curl.h>
#include "../../JSON/json.hpp"
#include "../../include/apikey_loader.h"
#include <string>

using json = nlohmann::json;

std::string api_key = getOpenAIKey();

// Helper function to handle response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to send string to ChatGPT for sentiment analysis
std::string analyzeSentiment(const std::string& inputText) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        const std::string url = "https://api.openai.com/v1/chat/completions";
        std::string data = json{
            {"model", "gpt-3.5-turbo"},
            {"messages", {{{"role", "user"}, {"content", "Analyze the sentiment (positive, negative, or neutral) of: " + inputText}}]},
            {"temperature", 0.2}
        }.dump();

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    try {
        auto responseJson = json::parse(readBuffer);
        std::string sentiment = responseJson["choices"][0]["message"]["content"];
        return sentiment;
    } catch (...) {
        return "Failed to parse sentiment.";
    }
}
