
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

void run_folds(std::set<std::pair<int, int>>& points, const std::vector<std::pair<char, int>>& instructions) {
    for (auto [ch, num] : instructions) {
        std::set<std::pair<int, int>> next_points {}; // We cannot erase and iterate at once.
        
        for (std::pair<int, int> pt : points) {
            switch (ch) {
            case 'x':
                if (pt.first > num)
                    next_points.insert({num - (pt.first - num), pt.second});
                else
                    next_points.insert(pt);
                break;
            case 'y':
                if (pt.second > num)
                    next_points.insert({pt.first, num - (pt.second - num)});
                else
                    next_points.insert(pt);
                break;
            default:
                assert(false && "Should see an x or a y here.");
            }
        }

        points = next_points;
    }
}

int main() {
    std::set<std::pair<int, int>> points {};

    while (true) {
        std::string buffer {};
        std::getline(std::cin, buffer);
        std::istringstream line {buffer};
        if (buffer == "")
            break;
       
        std::getline(line, buffer, ',');
        int i {std::stoi(buffer)};
        std::getline(line, buffer);
        int j {std::stoi(buffer)};

        points.insert({i, j});
    } 

    std::vector<std::pair<char, int>> fold_instructions {};
    
    while (true) {
        std::string buffer {};
        std::getline(std::cin, buffer);
        std::istringstream line {buffer};
        if (buffer == "")
            break;
        
        line >> buffer >> buffer >> buffer; // Buffer gets last important part

        fold_instructions.push_back({buffer[0], std::stoi(buffer.substr(2))});    
    }

    run_folds(points, fold_instructions);
    
    int max_x {0};
    int max_y {0};

    for (auto [x, y] : points) {
        if (x > max_x)
            max_x = x;
        if (y > max_y)
            max_y = y;
    }

    for (int row {0}; row < max_y + 1; row++) {
        for (int col {0}; col < max_x + 1; col++) {
            std::cout << (points.contains({col, row}) ? "⬜" : "⬛");
        }
        std::cout << '\n';
    }
}
