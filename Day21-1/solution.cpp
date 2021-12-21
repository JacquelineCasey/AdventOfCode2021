
#include <iostream>

class Die {
private:
    int rolls {0};

public:
    int get_rolls() { return rolls; }

    int roll() {
        int value {(rolls % 100) + 1};
        rolls++;

        return value;
    }
};

struct Player {
    int score {0};
    int space {}; // 1 to 10.
};

class Game {
private:
    Player player_1 {};
    Player player_2 {};
    Die die {};

public:
    void play_game(const int start_space_1, const int start_space_2) {
        player_1.space = start_space_1;
        player_2.space = start_space_2;

        while (true) {
            play_turn(player_1);
            if (is_over())
                break;

            play_turn(player_2);
            if (is_over())
                break;
        }
    }

    Player& get_loser() {
        if (player_1.score < 1000)
            return player_1;
        else
            return player_2;
    }

    Die& get_die() { return die; }

private:
    bool is_over() {
        return (player_1.score >= 1000 || player_2.score >= 1000);
    }

    void play_turn(Player& current_player) {
        int sum {die.roll() + die.roll() + die.roll()};
        current_player.space = ((current_player.space - 1 + sum) % 10) + 1;
        current_player.score += current_player.space;
    }    
};

int main() {
    int start_space_1 {};
    int start_space_2 {};    
    
    std::string buffer {};
    std::cin >> buffer >> buffer >> buffer >> buffer >> start_space_1;
    std::cin >> buffer >> buffer >> buffer >> buffer >> start_space_2;

    Game game {};
    game.play_game(start_space_1, start_space_2);

    std::cout << game.get_loser().score * game.get_die().get_rolls() << '\n';

    return 0;
}