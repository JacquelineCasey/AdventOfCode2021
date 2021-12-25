
#include <iostream>
#include <vector>

using Grid = std::vector<std::vector<char>>;

std::istream& operator>>(std::istream& in, Grid& grid) {
    grid = {};

    std::string buffer {};
    while (in >> buffer) {
        grid.push_back({});
        for (char ch : buffer) 
            grid.back().push_back(ch);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const Grid& grid) {
    for (const auto& row : grid) {
        for (char ch : row)
            out << ch;
        out << '\n';
    }

    return out;
}

/* Returns whether a move occurred. */
bool move_east(Grid& grid) {
    int r_size {static_cast<int>(grid.size())};
    int c_size {static_cast<int>(grid[0].size())};

    for (int row {0}; row < r_size; row++) {
        for (int col {0}; col < c_size; col++) {
            if (grid[row][col] == '>' && grid[row][(col + 1) % c_size] == '.') {
                grid[row][col] = 'o'; // out; 
                grid[row][(col + 1) % c_size] = 'i'; // in;
            }
        }
    }

    bool made_change {false};
    for (int row {0}; row < r_size; row++) {
        for (int col {0}; col < c_size; col++) {
            if (grid[row][col] == 'o') {
                grid[row][col] = '.';
                made_change = true;
            }
            else if (grid[row][col] == 'i')
                grid[row][col] = '>';
        }
    }

    return made_change;
}

/* Returns whether a move occurred. */
bool move_south(Grid& grid) {
    int r_size {static_cast<int>(grid.size())};
    int c_size {static_cast<int>(grid[0].size())};

    for (int row {0}; row < r_size; row++) {
        for (int col {0}; col < c_size; col++) {
            if (grid[row][col] == 'v' && grid[(row + 1) % r_size][col] == '.') {
                grid[row][col] = 'o'; // out; 
                grid[(row + 1) % r_size][col] = 'i'; // in;
            }
        }
    }

    bool made_change {false};
    for (int row {0}; row < r_size; row++) {
        for (int col {0}; col < c_size; col++) {
            if (grid[row][col] == 'o') {
                grid[row][col] = '.';
                made_change = true;
            }
            else if (grid[row][col] == 'i')
                grid[row][col] = 'v';
        }
    }

    return made_change;
}

int first_still_round(Grid& grid) {
    int round {0};

    while (true) {
        round++;
        bool east_moved {move_east(grid)};
        bool south_moved {move_south(grid)};
        if ((!east_moved) && (!south_moved))
            break;
    }

    return round;
}

int main() {
    Grid grid {};
    std::cin >> grid;
    std::cout << first_still_round(grid) << '\n';
    return 0;
}