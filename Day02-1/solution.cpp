
#include <iostream>

/* Unfortunately, switches do not support strings directly, so we use an enum.
 * We get to take advantage of C++'s operator overload system. */

enum direction {
    FORWARD,
    DOWN,
    UP
};

std::istream& operator>>(std::istream& in, direction& dir) {
    std::string buffer {};
    in >> buffer;

    if (buffer == "forward")
        dir = FORWARD;
    else if (buffer == "down")
        dir = DOWN;
    else if (buffer == "up")
        dir = UP;

    return in;
}

int main() {
    int x = {};
    int depth {};
    direction direction {};
    while (std::cin >> direction) {
        int amount {};
        std::cin >> amount;

        switch (direction) {
        case FORWARD:
            x += amount;
            break;
        case DOWN:
            depth += amount;
            break;
        case UP:
            depth -= amount;
            break;
        }
    }

    std::cout << "x: " << x << " depth: " << depth << "\n";
    std::cout << "answer: " << x * depth << "\n";
}