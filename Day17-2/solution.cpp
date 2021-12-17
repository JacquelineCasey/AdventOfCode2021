
/* 
 * Last time, for efficiency, we relied upon the particularities of our input.
 * This time, I am seeking a truly general solution (which implies a truly general
 * solution to part 1). 
 * */

/* 
 * We saw that when the whole target range is y negative, our vy0_max was the
 * |lower_bound| - 1. By a similar argument, if the range is y positive, the 
 * vy0_max is upper_bound (just shoot the corner in one step).
 * (Of course, we say that y always returns to 0, so any range that has a y = 0
 * will have an infinite number of solutions, so we are forced to ignore this
 * case). FOR SIMPLICITY, lets say that vy0_max is just the maximum absolute value
 * of the two bounds.
 * 
 * In our downward case, the probe takes at most 2 * (vy0_max + 1) steps. For an
 * upward case, it is a little less. FOR SIMPLICITY, assume 2 * (vy0_max + 1) steps.
 * After that many steps, y will never be in the range, so we can just ignore.
 * 
 * 
 * 
 * Generate a map (really a vector) from t's (t from 0 to t_max (which is 2 * (vy0_max + 1))) to
 * pairs of lists of vx0 values | vy0 values that have their t-th step in the 
 * target range. Generating this map is reasonably easy, and only requires simulating
 * on 1 dimension from the min possible to the max possible.
 * - Implementation wise, this map could really be two seperate maps.
 * 
 * For each pair in the map, form all combinations of vx0 and vy0, and drop them
 * in some set. We will get duplicates, but the set will handle that for us.
 * 
 * This should be a lot better than the naive approach, which is to simulate on
 * both axes for all combinations of x and y from (vx0_min to vx0_max) and (vy0_min to vy0_max).
 */

/* That being said, if the naive approach works, lets just do that. I am also
 * too lazy to parse tonight, so I'll just use some arrays or something. */

#include <array>
#include <cmath>
#include <iostream>

constexpr std::array<int, 4> target_range {230, 283, -107, -57}; // input
// constexpr std::array<int, 4> target_range {20, 30, -10, -5}; // sample

bool simulate(int vx, int vy, int t_steps) {
    int x {0};
    int y {0};
    for (int i {0}; i < t_steps; i++) {
        x += vx;
        y += vy;
        vy--;
        if (vx > 0) vx--;

        if (target_range[0] <= x && x <= target_range[1] && target_range[2] <= y && y <= target_range[3])
            return true;
    }

    return false;
}

int main() {
    int x_max {target_range[1]};
    int x_min {0};
    int x_stop_min; // Where x reaches 0 for x_min;
    while (x_stop_min < target_range[0]) {
        x_min++;
        x_stop_min += x_min;
    } 

    int y_max {std::abs(target_range[2])};
    int y_min {target_range[2]};

    int t_max {y_max * 2 + 2};

    int count {0};
    
    for (int x {x_min}; x <= x_max; x++) {
        for (int y {y_min}; y <= y_max; y++) {
            if (simulate(x, y, t_max))
                count++;
        }
    }

    std::cout << count << '\n';

    return 0;
}
