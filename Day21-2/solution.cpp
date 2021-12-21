
#include <iostream>
#include <map>

/* This requires dynamic programming. (Memoization) */

struct Player {
    int score {0};
    int space {}; // 1 to 10.
};

bool operator<(const Player& a, const Player& b) {
    if (a.score != b.score)
        return a.score < b.score;
    return a.space < b.space;
}

class Game {
private:
    /* Maps (current_player, next_player) -> (current_player_wins, next_player_wins)*/
    std::map<std::pair<Player, Player>, std::pair<long, long>> memo_map {};

public:
    /* Returns a pair: (wins for current player, wins for other player) */
    std::pair<long, long> play_game_from(Player current, Player next) {
        if (next.score >= 21)
            return {0, 1};

        if (memo_map.contains({current, next})) {
            return memo_map.at({current, next});
        }

        /* Compute part of the map. */
        long current_wins {0};
        long next_wins {0};
        for (int d_1 {1}; d_1 <= 3; d_1++) {
            for (int d_2 {1}; d_2 <= 3; d_2++) {
                for (int d_3 {1}; d_3 <= 3; d_3++) {
                    Player current_1 {current};
                    current_1.space = ((current_1.space - 1 + d_1 + d_2 + d_3) % 10) + 1;
                    current_1.score += current_1.space;

                    auto [n, c] = play_game_from(next, current_1);
                    current_wins += c;
                    next_wins += n;
                }
            }
        }

        memo_map.insert({{current, next}, {current_wins, next_wins}});
        return {current_wins, next_wins};
    }
};

int main() {
    int start_space_1 {};
    int start_space_2 {};    
    
    std::string buffer {};
    std::cin >> buffer >> buffer >> buffer >> buffer >> start_space_1;
    std::cin >> buffer >> buffer >> buffer >> buffer >> start_space_2;

    Game game {};
    auto [p1_score, p2_score] = game.play_game_from({0, start_space_1}, {0, start_space_2});
    std::cout << std::max(p1_score, p2_score) << '\n';

    return 0;
}