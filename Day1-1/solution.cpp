
#include <iostream> // For std::cin and std::cout

/* There are definitely sexier approaches involving zips, but why mess with 
 * iteration. */
int main() {

    int last_height {}; // always initialize with {} just in case.
    std::cin >> last_height; // This is standard input.

    /*
     * std::cin is an std::istream, and using >> on it returns another std:istream,
     * so you can chain them. (Technically, it returns a reference to itself).
     * Also, std::cin can be contextually converted to bool (it is true if it has not
     * yet failed). Therefore...
     */
    int next_height {};
    int increases = 0;
    while (std::cin >> next_height) { // try to read an int, and perform one loop iteration if you succeeded.
        if (next_height > last_height)
            increases++;

        last_height = next_height;
    }

    /* This is standard output. */
    std::cout << increases << '\n'; // always add '\n' or std::endl

    return 0;
}

// On my mac, g++ is actually clang for some reason. g++-11 is g++
// g++-11 -std=c++20 -Wall -Wextra -Werror -Wpedantic solution.cpp -o solution
// ./solution < input.txt