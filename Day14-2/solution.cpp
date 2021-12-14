
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

/* As predicted, this problem is exponential, and cannot be solved by brute force.
 * Essentially, we know we have to change the output, because a string with an
 * exponential number of characters is going to at least take an exponential 
 * number of steps to compute.
 * 
 * Instead, our function will build up a map of counts of characters. We can
 * memoize our function and hope for the best. The problem will have lots of
 * repeated calculations, but our memoization table will only have (# of rules * 
 * number of iterations) entries, so we will not do that much actual work.
 * 
 * The actual process will be recursive, so we can memoize calls to the function.
 * A better algorithm would be able to add more than one entry to the table per
 * early function call, but that is too much work for me and probably unnecessary.
 * 
 * Also, as always, we have to switch to using longs. */

std::map<char, long> add_maps(std::map<char, long> m1, const std::map<char, long>& m2) {
    for (auto [key1, val1] : m2) {
        m1[key1] += val1;
    }

    return m1; // Returns a copy, don't worry, neither original has changed.
}

/* It's global, but thats fine for now. */
std::map<std::pair<std::pair<char, char>, int>, std::map<char, long>> memoization_table {};

/* Returns the number of each character that is inserted between `chars` in `n` steps,
 * according to `rules` */
std::map<char, long> get_counts_between(
        const std::pair<char, char> chars, 
        const std::map<std::pair<char, char>, char>& rules, 
        int n) {

    /* Base Case: n = 0, or no insertion rule. */
    if (n == 0 || !rules.contains(chars)) {
        return {};
    }
    
    /* Check the memo table. */
    if (memoization_table.contains({chars, n})) {
        return memoization_table.at({chars, n});
    }

    auto [left, right] {chars};
    char inserted {rules.at(chars)};

    std::map<char, long> m1 {get_counts_between({left, inserted}, rules, n - 1)};
    std::map<char, long> m2 {get_counts_between({inserted, right}, rules, n - 1)};
    std::map<char, long> m_final {add_maps(m1, m2)};
    m_final[inserted]++;

    memoization_table[{chars, n}] = m_final;

    return m_final;
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

    /* Just to start, counts needs to contain info from original string. */
    std::map<char, long> counts {};
    for (char c : input_vector) {
        counts[c]++;
    }

    for (int i {1}; i < static_cast<int>(input_vector.size()); i++) {
        counts = add_maps(counts, get_counts_between({input_vector[i - 1], input_vector[i]}, rules, 40));
    }

    std::vector<long> counts_vector {};
    for (auto [_, count] : counts) {
        counts_vector.push_back(count);
    }

    std::sort(counts_vector.begin(), counts_vector.end());

    std::cout << counts_vector.back() - counts_vector[0] << '\n';
}
