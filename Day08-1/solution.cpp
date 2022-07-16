
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

/* Fill digits and to_decode with contents of line. Sort all strings. */
void parse_line(std::string line, std::vector<std::string>& digits, std::vector<std::string>& to_decode) {
    std::istringstream sstream {line};

    for (int i {0}; i < 10; i++) {
        std::string digit {};
        sstream >> digit;
        std::sort(digit.begin(), digit.end());
        digits.push_back(digit);
    }

    std::string _;
    sstream >> _; // | delimiter
    
    for (int i {0}; i < 4; i++) {
        std::string digit {};
        sstream >> digit;
        std::sort(digit.begin(), digit.end());
        to_decode.push_back(digit);
    }
}

using decoder_table = std::unordered_map<std::string, int>;

/* This function is currently simplified for part one. Strings we with non unique
 * lengths are set to -1 as a placeholder. */
decoder_table generate_decoder_table(const std::vector<std::string>& digits) {
    decoder_table table {};

    for (std::string str : digits) {
        int digit {};
        switch (str.length()) {
        case 2:
            digit = 1;
            break;
        case 3:
            digit = 7;
            break;
        case 4:
            digit = 4;
            break;
        case 7:
            digit = 8;
            break;
        default:
            digit = -1;
        }

        table[str] = digit;
    }

    return table;
}

std::vector<int> decode_with(const decoder_table& decoder, const std::vector<std::string>& encoded) {
    std::vector<int> output {};

    for (std::string str : encoded) {
        output.push_back(decoder.at(str));
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
        std::vector<int> results {decode_with(table, to_decode)};

        for (int item : results) {
            if (item != -1)
                total++;
        }
    }

    std::cout << "Total: " << total << "\n";

    return 0;
}