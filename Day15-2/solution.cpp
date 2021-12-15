
#include <array>
#include <iostream>
#include <queue>
#include <map>
#include <vector>

/* The only change involves setting up the array. The previous algorithm was strong
 * enough to survive a 25 fold increase in elements. */


/* node_queue sorts lexicorgraphically, putting small elements (low priorities) first. */
using queue_elem = std::pair<int, std::pair<int, int>>;
using node_queue = std::priority_queue<queue_elem, std::vector<queue_elem>, std::greater<queue_elem>>;

constexpr std::array<std::pair<int, int>, 4> adjacent {{
    {1, 0}, {0, 1}, {-1, 0}, {0, -1}
}};

int best_path_risk(const std::vector<std::vector<int>>& risk_levels) {
    int size {static_cast<int>(risk_levels.size())}; // The shape is a square.

    /* Representing the total risk to get to a given space. */
    std::map<std::pair<int, int>, int> known_risks {{{0, 0}, 0}};

    /* Representing the estimated risk of adjacent nodes. Stores (total_risk, (row, col))
     * values, sorted lexicorgraphically. May contain old entries correspond to 
     * old, innacurate estimates. Ignore these. */
    node_queue risk_estimates {};
    risk_estimates.push({risk_levels[0][1], {0, 1}});
    risk_estimates.push({risk_levels[1][0], {1, 0}});

    while (!known_risks.contains({size - 1, size - 1})) {
        auto [risk, pair] = risk_estimates.top();
        risk_estimates.pop();

        /* This may be an older entry with a worse risk... */
        if (known_risks.contains(pair))
            continue;

        known_risks[pair] = risk;
        
        for (auto [dr, dc] : adjacent) {
            int r {pair.first + dr};
            int c {pair.second + dc};
            if (r < 0 || r >= size || c < 0 || c >= size)
                continue;
            
            if (known_risks.contains({r, c}))
                continue;

            risk_estimates.push({risk + risk_levels[r][c], {r, c}});
        }
    }

    return known_risks[{size - 1, size - 1}];
}

int main() {
    std::vector<std::vector<int>> tile {};
    std::string buffer {};
    while (std::cin >> buffer) {
        tile.push_back({});
        for (char c : buffer) {
            tile.back().push_back(c - '0');
        }
    }

    const int tile_size {static_cast<int>(tile.size())}; 
    
    /* Construct a large and empty array first. */
    std::vector<std::vector<int>> risk_levels {};
    for (int i {0}; i < tile_size * 5; i++) {
        risk_levels.push_back(std::vector(tile_size * 5, 0)); // pushing vectors of 0s
    }

    /* Fill up the vector. */
    for (int row {0}; row < tile_size * 5; row++) {
        for (int col {0}; col < tile_size * 5; col++) {
            int tile_row = row / tile_size;
            int offset_row = row % tile_size;
            int tile_col = col / tile_size;
            int offset_col = col % tile_size;

            int risk_pre_wrap {tile[offset_row][offset_col] + tile_row + tile_col};
            risk_levels[row][col] = ((risk_pre_wrap - 1) % 9) + 1;
        }
    }

    std::cout << best_path_risk(risk_levels) << '\n';
}