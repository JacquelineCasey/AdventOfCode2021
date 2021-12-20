
/* So first things first: our encoding string starts with a #, meaning the 
 * infinite abyss of .... will be #### after one enhance, and flicker back
 * on the next one. We handle this by only simulating a (growing) square of
 * interest, and storing one value that represents 'everything else'. */

#include <array>
#include <deque> /* Double ended queue. Efficient insert at front and back. Perfect! */
#include <iostream>
#include <sstream>


struct EnhancementAlgorithm {
    std::array<bool, 512> lookup {};
};

class Image {
private:
    std::deque<std::deque<bool>> simulated_matrix {};
    bool everything_else {false};

public:
    friend std::istream& operator>>(std::istream& in, Image& image);

    bool get(int r, int c) {
        int n {static_cast<int>(simulated_matrix.size())};
        if (r < 0 || r >= n || c < 0 || c >= n)
            return everything_else;
        return simulated_matrix.at(r).at(c);
    }

    void enhance(const EnhancementAlgorithm& alg) {
        /* Expand the simulated region by one in each direction. */
        for (std::deque<bool>& row: simulated_matrix) {
            row.push_front(everything_else);
            row.push_back(everything_else);
        }
        int n {static_cast<int>(simulated_matrix[0].size())};
        simulated_matrix.push_front(std::deque<bool>(n, everything_else));
        simulated_matrix.push_back(std::deque<bool>(n, everything_else));

        std::deque<std::deque<bool>> next_matrix {simulated_matrix}; // Value semantics! This is a copy.
        for (int r {0}; r < n; r++) {
            for (int c {0}; c < n; c++) {
                int num {0};
                for (int dr {-1}; dr <= 1; dr++) {
                    for (int dc {-1}; dc <= 1; dc++) {
                        num *= 2;
                        num += get(r + dr, c + dc);
                    } 
                }
                next_matrix[r][c] = alg.lookup.at(num);
            }
        }

        simulated_matrix = next_matrix;
        everything_else = alg.lookup.at((everything_else ? 512 - 1 : 0));
    }

    int count_bright() {
        int count {0};
        for (auto& row : simulated_matrix) {
            for (auto item : row) {
                count += item;
            }
        }

        return count;
    }
};

std::istream& operator>>(std::istream& in, EnhancementAlgorithm& alg) {
    alg = {};

    for (int i {0}; i < 512; i++) {
        char ch {};
        in >> ch;
        alg.lookup[i] = (ch == '#');
    }

    return in;
}

std::istream& operator>>(std::istream& in, Image& image) {
    image = {};

    std::string buffer {};
    while (in >> buffer) {
        image.simulated_matrix.push_back({});
        std::istringstream sstream {buffer};

        char ch {};
        while (sstream >> ch) {
            image.simulated_matrix.back().push_back(ch == '#');
        }
    }
    
    return in;
}

int main() {
    EnhancementAlgorithm alg {};
    std::cin >> alg;

    Image image {};
    std::cin >> image;

    image.enhance(alg);
    image.enhance(alg);

    std::cout << image.count_bright() << '\n';

    return 0;
}
