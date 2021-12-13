
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>


/* I feel like this definitely is not asymptotically ideal, but with such small
 * input I don't think it really matters. */
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

    std::cout << "Points before fold: " << points.size() << '\n';

    std::vector<std::pair<char, int>> single_instruction {fold_instructions[0]};
    run_folds(points, single_instruction);

    std::cout << "Points after fold: " << points.size() << '\n';
}
