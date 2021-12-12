
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class CaveGraph {
private:
    std::unordered_map<std::string, std::vector<std::string>> adjacency_list {};

    bool is_small_cave(std::string str) {
        return str[0] >= 'a' && str[0] <= 'z';
    }

    /* Performs a depth-first-search style traversal.
     * Invariant: visited_small_caves will be reset by the time the function completes. */
    int count_from(std::string curr_node, std::unordered_set<std::string>& visited_small_caves) {
        /* If we have reached the end, return 1. */
        if (curr_node == "end")
            return 1;

        /* If curr_node is a small cave, make note or return 0 (invalid path from here). */
        if (is_small_cave(curr_node)) {
            if (visited_small_caves.contains(curr_node))
                return 0;
            visited_small_caves.insert(curr_node);
        }

        /* Sum up successful paths from neighbors. */
        int sum {0};
        for (std::string adjacent : adjacency_list[curr_node]) {
            sum += count_from(adjacent, visited_small_caves);
        }

        /* Reset visited_small_caves set if necessary. */
        if (is_small_cave(curr_node))
            visited_small_caves.erase(curr_node);

        return sum;
    }

public:
    void add_edge(std::string s1, std::string s2) {
        adjacency_list[s1].push_back(s2);
        adjacency_list[s2].push_back(s1);
    }

    int count_paths() {
        std::unordered_set<std::string> visited_small_caves {};
        return count_from("start", visited_small_caves);
    }
};

int main() {
    CaveGraph graph {};

    std::string line {};
    while (std::cin >> line) {
        int dash_index {static_cast<int>(line.find('-'))};
        graph.add_edge(line.substr(0, dash_index), line.substr(dash_index + 1));
    }

    std::cout << graph.count_paths() << '\n';

    return 0;
}