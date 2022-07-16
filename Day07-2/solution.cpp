
/* The cost function is now triangular numbers, which almost matches the sum of
 * square error metric in statistics (this would be minimized by the mean). 
 * This does not mean that the mean (tee hee) will work, but it is a good starting
 * point if we need it (locate the minimum, ie stop when the cost increases;
 * check both directions). 
 * 
 * It seems we don't have a nice mathmatical solution, so we have to just code it. 
 * This seems like a fairly time complex task, so we have some tricks: 
 * - memoize the cost function: store costs as we first find them. 
 * 
 * I am considering a binary search style solution, but it may be overkill. I 
 * start by just computing the total cost for each position between the lowest
 * and highest position. O(distance * crabs).
 * 
 * This turned out to be fast enough (probably thanks to the memoization). */


#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

/* Memoized for efficiency. */
int fuel_cost(int distance) {
    static std::unordered_map<int, int> cached_costs {{0, 0}}; // Careful: you need the base case here (0 -> 0)

    if (cached_costs.contains(distance)) {
        return cached_costs[distance];
    }

    cached_costs[distance] = distance + fuel_cost(distance - 1);
    return cached_costs[distance];
}

int main() {
    std::vector<int> positions {};

    int num {};
    char _;
    while (std::cin >> num) {
        positions.push_back(num);
        std::cin >> _; // ','
    }

    std::sort(positions.begin(), positions.end());
    int lowest_pos {positions[0]};
    int highest_pos {positions[positions.size() - 1]};

    int best_deviation {INT32_MAX};
    for (int i {lowest_pos}; i <= highest_pos; i++) {
        int current_cost {};
        for (int pos : positions) {
            current_cost += fuel_cost(std::abs(pos - i));
        }

        if (current_cost > best_deviation)
            break;

        best_deviation = current_cost;
    }

    std::cout << "Fuel Cost: " << best_deviation << '\n';

    return 0;
}

