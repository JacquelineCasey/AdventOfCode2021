
#include <array>
#include <iostream>
#include <queue>
#include <map>
#include <vector>

/* Essentially Dijkstra's Algorithm, except the weight of an edge is the amount
 * stored in the node. NOTE: since the edges of bidirectional, the weight cannot
 * be two numbers, so in reality we treat the weight as the value in the node of
 * whichever node we can get to last. This makes this more of a modification of
 * Dijkstra, which is fine, because I am coding it from memory / scratch. */


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

    while (!known_risks.contains({size - 1, size -1})) {
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
    std::vector<std::vector<int>> risk_levels {};
    std::string buffer {};
    while (std::cin >> buffer) {
        risk_levels.push_back({});
        for (char c : buffer) {
            risk_levels.back().push_back(c - '0');
        }
    }

    std::cout << best_path_risk(risk_levels) << '\n';
}