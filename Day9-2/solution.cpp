
#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

/* Importantly, all basins correspond to a single low point. We can generate the 
 * low points, as before, and the try to expand them out into basins. Since all
 * low points have a basin, we can simply include all non-9 squares (instead of
 * only checking increasing sections from the lowpoint). */

class HeightMap {
private:
    const std::vector<std::vector<int>> matrix {};
    std::vector<std::pair<int, int>> low_points {};
    std::vector<int> basin_sizes {};
    int height;
    int width;

    /* No corners! */
    static constexpr std::array<std::pair<int, int>, 4> adjacent_shift {  
      { {-1,  0},
        { 0, -1},
        { 0,  1},
        { 1,  0} }
    };

    void calculate_low_points() {
        low_points = {};
        for (int row {0}; row < height; row++) {
            for (int col {0}; col < width; col++) {
                bool is_low_point {true};
                for (auto [d_row, d_col] : adjacent_shift) {
                    int r = row + d_row;
                    int c = col + d_col;
                    if (r < 0 || r >= height || c < 0 || c >= width)
                        continue;
                    if (matrix[r][c] <= matrix[row][col]) {
                        is_low_point = false;
                        break;
                    }
                }

                if (is_low_point) {
                    low_points.push_back({row, col});
                }
            }
        }
    }

    void calculate_basin_sizes() {
        for (auto [row, col] : low_points) {
            basin_sizes.push_back(get_basin_size_around(row, col));
        }
    }

    int get_basin_size_around(int lowpoint_row, int lowpoint_col) {
        std::vector<std::pair<int, int>> unchecked_squares {{lowpoint_row, lowpoint_col}};
        std::set<std::pair<int, int>> squares_in_basin {{lowpoint_row, lowpoint_col}};

        while (!unchecked_squares.empty()) {
            auto [row, col] = unchecked_squares.back();
            unchecked_squares.pop_back();

            for (auto [d_row, d_col] : adjacent_shift) {
                int r = row + d_row;
                int c = col + d_col;
                if (r < 0 || r >= height || c < 0 || c >= width)
                    continue;

                if (!squares_in_basin.contains({r, c}) && matrix[r][c] != 9) {
                    squares_in_basin.insert({r, c});
                    unchecked_squares.push_back({r, c});
                }
            }
        }

        return squares_in_basin.size();
    }

public:
    /* Assumes a square matrix as input. */
    HeightMap(std::vector<std::vector<int>> matrix) : 
            matrix {matrix},
            height {static_cast<int>(matrix.size())},
            width {static_cast<int>(matrix[0].size())} {
        calculate_low_points(); 
        calculate_basin_sizes();
    }

    /* Part 1. */
    int sum_risk_level() {
        int sum {0};
        for (auto [row, col] : low_points)
            sum += matrix[row][col] + 1;

        return sum;
    }

    /* For Part 2. */
    std::vector<int> get_basin_sizes() {
        return basin_sizes;
    }


};

int main() {
    std::string buffer {};
    std::vector<std::vector<int>> input {};
    while (std::getline(std::cin, buffer)) {
        
        std::vector<int> curr_row {};
        std::istringstream sstream {buffer};

        char digit {};
        while (sstream >> digit) {
            curr_row.push_back(std::stoi(std::string {digit}));
        }

        input.push_back(curr_row);
    }

    HeightMap height_map {input};
    std::vector<int> basin_sizes {height_map.get_basin_sizes()};
    std::sort(basin_sizes.rbegin(), basin_sizes.rend()); // Largest to smallest (reverse of normal)
    std::cout << "Answer: " << basin_sizes[0] * basin_sizes[1] * basin_sizes[2] << '\n';
}