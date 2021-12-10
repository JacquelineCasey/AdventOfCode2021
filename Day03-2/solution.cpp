
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <ranges>
#include <vector>


constexpr int NUM_BITS = 12;

/* Returns which bit ought to be kept. Remaining is the list of remaining entries. 
 * index is the current bit index we are on. wants_greater determins whether the
 * statistic will have us keep the greater or smaller group. */
bool prefered_bit(const std::vector<std::bitset<NUM_BITS>>& remaining, int index, bool wants_greater) {
    int ones_count {};
    int zeros_count {};
    for (int i {0}; i < static_cast<int>(remaining.size()); i++) {
        ones_count += remaining[i][index];
        zeros_count += !remaining[i][index];
    }

    if (wants_greater)
        return ones_count >= zeros_count;
    return ones_count < zeros_count;
}

std::bitset<NUM_BITS> find_line(const std::vector<std::bitset<NUM_BITS>>& input, bool wants_greater) {
    auto remaining = std::vector<std::bitset<NUM_BITS>>(input);
    for (int index {NUM_BITS - 1}; remaining.size() > 1; index--) { // Note: the leftmost bit is actually the last index!
        bool look_for = prefered_bit(remaining, index, wants_greater);

        std::vector<std::bitset<NUM_BITS>> next_remaining {};
        std::ranges::copy_if(remaining, std::back_inserter(next_remaining), [=](auto current) {
            return current[index] == look_for;
        });

        remaining = next_remaining;
    }

    return remaining[0];
}

int main() {
    std::vector<std::bitset<NUM_BITS>> input {};

    std::bitset<NUM_BITS> next {};
    while (std::cin >> next) {
        input.push_back(next);
    }

    std::bitset<NUM_BITS> oxygen = find_line(input, true);
    std::cout << "Oxygen: " << oxygen << std::endl;

    std::bitset<NUM_BITS> CO2 = find_line(input, false);
    std::cout << "CO2: " << CO2 << std::endl;

    std::cout << "Answer: " << oxygen.to_ulong() * CO2.to_ullong()  << std::endl;
}
