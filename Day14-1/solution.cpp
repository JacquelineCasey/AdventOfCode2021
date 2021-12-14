
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

/* You can already see that this problem explodes exponentially with time:
 * each step has the potential to introduce a number of new nodes that is almost
 * equal to the number of nodes in the last step.
 * 
 * For part 1, I want to get parsing and a simple brute force solution */

void substitute_n_times(std::vector<char>& vec, const std::map<std::pair<char, char>, char>& rules, int n) {
    for (int i {0}; i < n; i++) {
        std::vector<char> next_vec {vec[0]};
        for (int index {1}; index < static_cast<int>(vec.size()); index++) {
            if (rules.contains({vec[index - 1], vec[index]})) {
                next_vec.push_back(rules.at({vec[index - 1], vec[index]}));
            }

            next_vec.push_back(vec[index]);
        }

        vec = next_vec;
    }
}

int main() {
    std::string input {};
    std::cin >> input;
    std::vector<char> input_vector(input.begin(), input.end());

    std::map<std::pair<char, char>, char> rules {};
    std::string left {};
    std::string arrow {};
    std::string right {};
    while (std::cin >> left >> arrow >> right) {
        rules[{left[0], left[1]}] = right[0];
    }

    substitute_n_times(input_vector, rules, 10);

    std::map<char, int> counts {};
    for (char c : input_vector) {
        counts[c]++;
    }

    std::vector<int> counts_vector {};
    for (auto [_, count] : counts) {
        counts_vector.push_back(count);
    }

    std::sort(counts_vector.begin(), counts_vector.end());

    std::cout << counts_vector.back() - counts_vector[0] << '\n';
}
