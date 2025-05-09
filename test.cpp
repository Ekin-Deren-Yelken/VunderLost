#include "include/chatgpt_wrapper.h"
#include <iostream>


int main() {
    std::string input = "I'm feeling really great about this!";
    std::string result = analyzeSentiment(input);
    std::cout << "Sentiment: " << result << std::endl;
    return 0;
}