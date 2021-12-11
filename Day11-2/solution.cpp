
#include <array>
#include <iostream>


class SquidGrid {
public:
    constexpr static int SIZE {10};
    constexpr static int THRESHOLD {10}; // Energy Level that triggers a flash.

private:
    constexpr static std::array<std::pair<int, int>, 8> adjacencies {
      { {-1, -1},
        {-1,  0},
        {-1,  1},
        { 0, -1},
        { 0,  1},
        { 1, -1},
        { 1,  0},
        { 1,  1} }
    };

    std::array<std::array<int, SIZE>, SIZE> energies {};
    int total_flashes {0};

    /* May cause flashes and further increments. */
    void increment_at(int row, int col) {
        /* Bounds checking here simplifies the logic. */
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE)
            return;

        energies[row][col]++;
        if (energies[row][col] == THRESHOLD) { // Only if we JUST hit the threshold.
            total_flashes++;
            for (auto[dr, dc] : adjacencies)
                increment_at(row + dr, col + dc);
        }
    }

public:
    friend std::istream& operator>>(std::istream& in, SquidGrid& grid);

    int get_total_flashes() { return total_flashes; }

    void step() {
        /* Perform energy update. */
        for (int i {0}; i < SIZE; i++)
            for (int j {0}; j < SIZE; j++) 
                increment_at(i, j); // This includes flashes


        /* Perform reset to 0 */
        for (int i {0}; i < SIZE; i++)
            for (int j {0}; j < SIZE; j++)
                if (energies[i][j] >= THRESHOLD)
                    energies[i][j] = 0;
    }
};

std::istream& operator>>(std::istream& in, SquidGrid& grid) {
    grid = {};

    char buf {};
    for (int i {0}; i < SquidGrid::SIZE; i++) {
        for (int j {0}; j < SquidGrid::SIZE; j++) {
            in >> buf;
            grid.energies[i][j] = buf - '0';
        }
    }

    return in;
}

/* The only changes are down here! */
int main() {
    SquidGrid sg {};
    std::cin >> sg;

    int steps {0};
    while (true) {
        steps++;
        int old_flashes {sg.get_total_flashes()};
        sg.step();
        if (sg.get_total_flashes() - old_flashes == SquidGrid::SIZE * SquidGrid::SIZE)
            break;
    }

    std::cout << "Steps until synchronized: " << steps << '\n';

    return 0;
}
