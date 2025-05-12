#include "include/rpg_utils.h"
#include <iostream>
#include <algorithm>
#include <cctype>  


int main() {
    bool sentimentChecked = false;
    while (!sentimentChecked) {
        std::string hc;
        std::cout << "What do you say > ";
        std::cin >> hc;

        if (hc == "quit") {
            sentimentChecked = true;
        }


    }
}