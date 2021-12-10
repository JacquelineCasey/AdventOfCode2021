
#include <iostream>

/* There are definitely sexier approaches involving zips, but why mess with 
 * iteration. */
int main() {

    int last_height {};
    std::cin >> last_height;

    int next_height {};
    int increases = 0;
    /* std::istream converts to true if it has not yet failed. */
    while (std::cin >> next_height) { // try to read an int, and perform one loop iteration if you succeeded.
        if (next_height > last_height)
            increases++;

        last_height = next_height;
    }

    std::cout << increases << '\n';

    return 0;
}

// On my mac, g++ is actually clang for some reason. g++-11 is g++
// g++-11 -std=c++20 -Wall -Wextra -Werror -Wpedantic solution.cpp -o solution
// ./solution < input.txt