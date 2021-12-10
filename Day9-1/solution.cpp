
#include <array>
#include <iostream>
#include <sstream>
#include <vector>

class HeightMap {
private:
    std::vector<std::vector<int>> matrix {};
    std::vector<std::pair<int, int>> low_points {};
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

public:
    /* Assumes a square matrix as input. */
    HeightMap(std::vector<std::vector<int>> matrix) : 
            matrix {matrix},
            height {static_cast<int>(matrix.size())},
            width {static_cast<int>(matrix[0].size())} {
        calculate_low_points(); 
    }

    int sum_risk_level() {
        int sum {0};
        for (auto [row, col] : low_points)
            sum += matrix[row][col] + 1;

        return sum;
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
    std::cout << height_map.sum_risk_level() << '\n';
}