
/* To simulate each fish is a fools errand: we know there numbers grow exponentially,
 * so the time of the calculation would also grow exponentially:
 * We can try to see what happens for just one fish, and deduce what each fish
 * in the initial group results in (this is still exponential, but better).
 * OR we can track groups of fish (this is O(lifecycle * days)). This is what I chose to do. */

/* The code is easily adjustable for parts one and two. You did need to switch to
 * longs in order to fit the result of part two, though. */

#include <array>
#include <iostream>

/* Returns the number shifted out the left side. */
template<typename T, unsigned long NUM> 
T shift_left(std::array<T, NUM>& arr) {
    /* Swawp adjacent elements from left to right, moving all left except for leftmost. */
    for (int i {0}; i < static_cast<T>(NUM) - 1; i++) {
        std::swap(arr[i], arr[i + 1]);
    }

    /* What was arr[0] is now in arr[NUM - 1]; zero arr[NUM - 1] and return that value. */
    T retval {arr[NUM - 1]};
    arr[NUM - 1] = 0;

    return retval;
}

constexpr int DAYS {256}; // Set to 80 for part 1.

int main() {
    std::array<long, 9> number_at_time {};
    long input {};
    while (std::cin >> input) {
        char _ {};
        std::cin >> _; // ','
        number_at_time[input]++;
    }

    for (int i {0}; i < DAYS; i++) {
        long reproducing = shift_left(number_at_time);
        number_at_time[6] += reproducing;
        number_at_time[8] += reproducing;
    }

    long sum {0};
    for (long item : number_at_time) {
        sum += item;
    }

    std::cout << "Total: " << sum << "\n";
}