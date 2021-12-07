
/* Our goal is to minimize the absolute deviations from some value that we align
 * to. We have to return the deviations (not the value we align to). 
 * THIS IS STATISTICS! In statistics, we discuss several measures of spread, and
 * one is the sum of absolute deviations. The value that minimizes this cost is
 * the median.
 * 
 * The solution therefore is to find the median (sort) and then to calculate the
 * sum of absolute distances to that median. 
 * 
 * "What about the case where there are an even number of points? We must align 
 * to an int!" In this case, both middle points function perfectly well as medians,
 * which also applies to any point in between. Just use whichever of the two is
 * more convenient. */

#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> positions {};

    int num {};
    char _;
    while (std::cin >> num) {
        positions.push_back(num);
        std::cin >> _; // ','
    }

    std::ranges::sort(positions);
    int median = positions[positions.size() / 2];

    int total_deviation {0};
    for (int pos : positions) {
        total_deviation += std::abs(median - pos);
    }

    std::cout << "Fuel Cost: " << total_deviation << '\n';

    return 0;
}

