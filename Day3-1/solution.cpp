
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>


constexpr int NUM_BITS = 12;

int main() {
    std::vector<std::bitset<NUM_BITS>> input {};

    std::bitset<NUM_BITS> next {};
    while (std::cin >> next) {
        input.push_back(next);
    }
    
    std::array<int, NUM_BITS> ones_in_column {};
    std::for_each(input.begin(), input.end(), [&ones_in_column](const std::bitset<NUM_BITS>& current) {
        for (int i {0}; i < NUM_BITS; i++) {
            ones_in_column[i] += current[i];
        }
    });

    std::bitset<NUM_BITS> most_common {}; 
    for (int i {0}; i < NUM_BITS; i++) {
        most_common[i] = (unsigned) ones_in_column[i] > input.size() / 2;
    }
    
    int gamma_rate = (int) most_common.to_ulong();
    int epsilon_rate = (int) (~most_common).to_ulong();

    std::cout << "gamma rate: " << gamma_rate << " epsilon rate: " << epsilon_rate << "\n";
    std::cout << "power consumption: " << gamma_rate * epsilon_rate << "\n";
}