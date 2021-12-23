
#include <array>
#include <compare>
#include <iostream>
#include <map>
#include <vector>

/* The input has been updated manually. */

constexpr std::array<int, 7> SPOTS {0, 1, 3, 5, 7, 9, 10};

struct Room {
public:
    std::array<char, 4> front_to_back {};

    int x_loc {};
};

constexpr std::array<Room, 4> start_rooms {
  { {'A', '.', '.', 2},
    {'B', '.', '.', 4},
    {'C', '.', '.', 6},
    {'D', '.', '.', 8}
  }
};

constexpr std::array<int, 4> energies {
    1, 10, 100, 1000
};

class State {
public:
    std::map<int, char> hallway_spots {};
    std::array<Room, 4> rooms {};
    
    void init(std::array<char, 16> fish) {
        for (int i : SPOTS) 
            hallway_spots.insert({i, '.'});

        for (int i {0}; i < 4; i++) {
            rooms[0].front_to_back[i] = fish[4*i + 0];
            rooms[1].front_to_back[i] = fish[4*i + 1];
            rooms[2].front_to_back[i] = fish[4*i + 2];
            rooms[3].front_to_back[i] = fish[4*i + 3];
        }

        rooms[0].x_loc = 2;
        rooms[1].x_loc = 4;
        rooms[2].x_loc = 6;
        rooms[3].x_loc = 8;
    }

    friend std::strong_ordering operator<=>(const State& lhs, const State& rhs) {
        if (lhs.hallway_spots != rhs.hallway_spots)
            return lhs.hallway_spots <=> rhs.hallway_spots;
        for (int i {0}; i < 4; i++) {
            if (lhs.rooms[i].front_to_back != rhs.rooms[i].front_to_back)
                return lhs.rooms[i].front_to_back <=> rhs.rooms[i].front_to_back;
        }

        return std::strong_ordering::equal;
    }

    friend std::ostream& operator<<(std::ostream& out, const State& state) {
        out << "#############\n";

        out << '#';
        for (int i {0}; i <= 10; i++) {
            if (!state.hallway_spots.contains(i))
                out << '.';
            else
                out << state.hallway_spots.at(i);
        }
        out << "#\n";

        out << "###";
        for (int i {0}; i < 4; i++)
            out << state.rooms[i].front_to_back[0] << '#';
        out << "##\n";

  
        for (int r {1}; r < 4; r++) {
            out << "  #";
            for (int i {0}; i < 4; i++)
                out << state.rooms[i].front_to_back[r] << '#';
            out << '\n';
        }

        return out << "  #########\n";
    }

    bool is_done() const {
        for (int i {0}; i < 4; i++) {
            for (int item : rooms[i].front_to_back)
                if (item != i + 'A')
                    return false;
        }
        return true;
    }

    /* Returns if the fish can go home unblocked. From (x, 0) */
    // Now returns (can_go_home, cost, y)
    std::tuple<bool, int, int> can_go_home(char fish, const int x0) const {
        /* false if home is not open, or contains wrong fish, or in home. */
        const Room& room {rooms[fish - 'A']};

        if (room.x_loc == x0)
            return {false, -123456789, -99};
        
        if (room.front_to_back[0] != '.')
            return {false, -123456789, -99};
        
        for (int i {1}; i < 4; i++)
            if (room.front_to_back[i] != '.' && room.front_to_back[i] != fish)
                return {false, -123456789, -99};

        int x_final {room.x_loc};
        int dx {(x_final - x0 > 0) ? 1 : -1};

        for (int x {x0 + dx}; x != x_final; x += dx) {
            if (!hallway_spots.contains(x))
                continue;
            if (hallway_spots.at(x) != '.')
                return {false, -123456789, -99};
        }
        
        int cost = energies[fish - 'A'] * std::abs(x_final - x0);
        int y {4};
        while (room.front_to_back.at(y - 1) != '.') {
            y--;
        }
        cost += y * energies[fish - 'A'];
        
        return {true, cost, y};
    }

    std::vector<int> hallway_moves(int x0) const {
        std::vector<int> available_moves {};

        /* Left */
        for (int x {x0 - 1}; x >= 0; x--) {
            if (!hallway_spots.contains(x))
                continue;
            if (hallway_spots.at(x) != '.')
                break;
            available_moves.push_back(x);
        }

        /* Right */
        for (int x {x0 + 1}; x <= 10; x++) {
            if (!hallway_spots.contains(x))
                continue;
            if (hallway_spots.at(x) != '.')
                break;
            available_moves.push_back(x);
        }

        return available_moves;
    }

    /* State and cost */
    std::vector<std::pair<State, int>> next_states() const {

        std::vector<std::pair<State, int>> output {}; 

        /* For fish in the hallways, try to move them to their room. */
        for (auto [x_pos, ch] : hallway_spots) {
            if (ch == '.')
                continue;

            auto [going_home, cost, y] {can_go_home(ch, x_pos)};
            if (going_home) {
                State next_state {*this};

                next_state.hallway_spots.at(x_pos) = '.';

                next_state.rooms[ch - 'A'].front_to_back.at(y-1) = ch;

                output.push_back({next_state, cost});
            }
        }

        /* For fish in the rooms, try to move them to their home rooms. */
        for (int room_index {0}; room_index < 4; room_index++) {
            const Room& room = rooms[room_index];

            for (int i {0}; i < 4; i++) {
                int y0 {i + 1};
                char ch {room.front_to_back[i]};
                
                if (ch == '.')
                    continue; // Nothing here.

                if (ch - 'A' == room_index) {
                    bool skip {true};
                    for (int i_temp {i}; i_temp < 4; i_temp++) {
                        if (room.front_to_back[i_temp] != ch)
                            skip = false; // Not packed.
                    }
                    if (skip) continue; // Fish is home (packed towards back).
                }

                bool skip {false};
                for (int i_temp {i - 1}; i_temp >= 0; i_temp--) {
                    if (room.front_to_back[i_temp] != '.')
                        skip = true; // Fish cannot move out.
                }
                if (skip) continue;
                
                auto [going_home, home_cost, y_final] {can_go_home(ch, room.x_loc)};
                if (going_home) {
                    State next_state {*this};
                    home_cost += energies[ch - 'A'] * y0; // Cost to move out of room.

                    next_state.rooms[room_index].front_to_back[i] = '.';

                    next_state.rooms[ch - 'A'].front_to_back[y_final - 1] = ch;

                    output.push_back({next_state, home_cost});
                }

                /* Otherwise attempt moves into the hallway */
                else {
                    std::vector<int> moves {hallway_moves((room_index + 1) * 2)};
                    for (int x_final : moves) {
                        State next_state {*this};
                        int hallway_cost {energies[ch - 'A'] * y0}; // Cost to move out of room
                        hallway_cost += energies[ch - 'A'] * std::abs(((room_index + 1) * 2) - x_final);

                        next_state.rooms[room_index].front_to_back[i] = '.';

                        next_state.hallway_spots.at(x_final) = ch;

                        output.push_back({next_state, hallway_cost});
                    }
                    
                }
            }   
        }
        
        return output;
    }
};
std::map<State, int> memo_map {};
int find_lowest_cost_from(const State& state) {
    if (state.is_done())
        return 0;

    if (memo_map.contains(state))
        return memo_map.at(state);

    auto states {state.next_states()};

    int min_cost {INT32_MAX};
    for (const auto& [s, cost] : states) {
        
        int rest_cost {find_lowest_cost_from(s)};

        if (rest_cost == -1)
            continue;
        
        int total_cost {cost + rest_cost};

        if (total_cost < min_cost)
            min_cost = total_cost;
    }

    if (min_cost == INT32_MAX)
        min_cost = -1; // We got stuck at all next moves.

    memo_map.insert({state, min_cost});

    return min_cost;
}

int main() {
    std::string str {};
    char ch {};
    std::array<char, 16> input;
    std::cin >> str 
             >> str
             >> ch >> ch >> ch >> input[0] >> ch >> input[1] >> ch >> input[2] >> ch >> input[3] >> str
             >> ch >> input[4] >> ch >> input[5] >> ch >> input[6] >> ch >> input[7] >> str
             >> ch >> input[8] >> ch >> input[9] >> ch >> input[10] >> ch >> input[11] >> str
             >> ch >> input[12] >> ch >> input[13] >> ch >> input[14] >> ch >> input[15] >> str;

    State state {};
    state.init(input);

    std::cout << find_lowest_cost_from(state) << '\n';

    return 0;
}
