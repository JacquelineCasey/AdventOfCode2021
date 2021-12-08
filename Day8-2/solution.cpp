
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

/* The key insight is that we have information on the total frequency of segments
 * across all 10 digits. If we count them all, they have mostly unique frequencies,
 * but 2 appear 8 times and 2 appear 7 times, so this is not yet perfect.
 * Incidentally though, if we convert each number to a (sorted) list of frequencies
 * of its segments, we can simply look up what digit it actually is. This is lucky,
 * but we can use this coincidence to have a fairly fast algorithm that does no
 * guess and check (and no drawn out logic). We don't even have to explicitely
 * track down 1 4 7 and 8 now, like we did in part 1. */

/* Fill digits and to_decode with contents of line. Sort all strings. */
void parse_line(std::string line, std::vector<std::string>& digits, std::vector<std::string>& to_decode) {
    std::istringstream sstream {line};

    for (int i {0}; i < 10; i++) {
        std::string digit {};
        sstream >> digit;
        std::ranges::sort(digit);
        digits.push_back(digit);
    }

    std::string _;
    sstream >> _; // | delimiter
    
    for (int i {0}; i < 4; i++) {
        std::string digit {};
        sstream >> digit;
        std::ranges::sort(digit);
        to_decode.push_back(digit);
    }
}

using decoder_table = std::unordered_map<std::string, int>;

/* Luckily, no segment appears all 10 times, so we can use strings. */
const std::unordered_map<std::string, int> segment_frequencies_to_digit {
    {"467889", 0},
    {"89", 1},
    {"47788", 2},
    {"77889", 3},
    {"6789", 4},
    {"67789", 5},
    {"467789", 6},
    {"889", 7},
    {"4677889", 8},
    {"677889", 9},
};

decoder_table generate_decoder_table(const std::vector<std::string>& digits) {
    decoder_table table {};
    std::unordered_map<char, int> char_frequencies {};

    /* Build the table of character frequencies. */
    for (const std::string& str : digits) {
        for (char ch : str) {
            char_frequencies[ch]++;
        }
    }

    /* Convert each string to a (sorted) list of frequencies, and use the lookup table. */
    for (const std::string& str : digits) {
        std::string frequencies_str {};
        for (char ch : str) {
            frequencies_str += std::to_string(char_frequencies[ch]);
        }
        std::ranges::sort(frequencies_str);
        table[str] = segment_frequencies_to_digit.at(frequencies_str);
    }

    return table;
}

/* This just needs to return an int now. */
int decode_with(const decoder_table& decoder, const std::vector<std::string>& encoded) {
    int output {0};

    for (std::string str : encoded) {
        output *= 10;
        output += decoder.at(str);
    }

    return output;
}

int main() {
    int total {0};

    std::string line {};
    while (std::getline(std::cin, line)) {
        std::vector<std::string> digits {};
        std::vector<std::string> to_decode {};
        parse_line(line, digits, to_decode);

        decoder_table table {generate_decoder_table(digits)};
        int result {decode_with(table, to_decode)};

        total += result;
    }

    std::cout << "Total: " << total << "\n";

    return 0;
}