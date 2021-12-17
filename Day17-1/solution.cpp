
/*
 * target area: x=230..283, y=-107..-57
 *
 * The target has a negative y positition. Observe that in this case, the best
 * trajectory is the one with the highest y value that still enters the target.
 *
 * We can analyze both the x and y dimensions piece by piece. Observe that probe
 * always returns to y = 0 after some point. After that, it will move down
 * 1 + vy0. vy0 is thus bounded from above by the absolute value of the lowest
 * point of the target square, minus 1 (any higher, and the trajectory is 
 * gauranteed to miss).
 * 
 * That is our candidate, but we have not proven that there is a successful 
 * trajectory with that vy0.
 * 
 * Observe that the probe eventually stops moving in the x direction, on a
 * x value equal to a triangular number (corresponding to vx0). If our range has
 * an accomodating triangular number, we can hope to get vx to be zero by the
 * time y enters the range.
 * 
 * My range supports several of these triangular numbers. Will vx reach 0 in time?
 * 
 * It takes at least 2vy0 steps for y to be 0 again. x = 21 will stop on the 21st
 * step (at 231), which is much lower than our candidate vy0 (106). So our candidate
 * works!
 * 
 * How high does the probe go with vy0 = 106? That is simply the 106th triangular
 * number
 */

#include <iostream>

int main() {
    int sum {};
    for (int i {1}; i <= 106; i++)
        sum += i;

    std::cout << sum << '\n';
}