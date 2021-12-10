
#include <iostream>

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
    int aim {};
    direction direction {};
    while (std::cin >> direction) {
        int amount {};
        std::cin >> amount;

        /* The only real changes were in these cases. */
        switch (direction) {
        case FORWARD:
            x += amount;
            depth += aim * amount;
            break;
        case DOWN:
            aim += amount;
            break;
        case UP:
            aim -= amount;
            break;
        }
    }

    std::cout << "x: " << x << " depth: " << depth << "\n";
    std::cout << "answer: " << x * depth << "\n";
}