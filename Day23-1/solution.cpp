
#include <array>
#include <compare>
#include <iostream>
#include <map>
#include <vector>

constexpr std::array<int, 7> SPOTS {0, 1, 3, 5, 7, 9, 10};

struct Room {
public:
    char desired {};
    char front {}; // '.' if empty
    char back {}; // '.' if empty

    int x_loc {};
};

// constexpr std::array<Room, 4> start_rooms {
//   { {'A', '.', '.', 2},
//     {'B', '.', '.', 4},
//     {'C', '.', '.', 6},
//     {'D', '.', '.', 8}
//   }
// };

constexpr std::array<int, 4> energies {
    1, 10, 100, 1000
};


class State {
public:
    std::map<int, char> hallway_spots {};
    std::array<Room, 4> rooms {};
    
    void init(std::array<char, 8> fish) {
        for (int i : SPOTS) 
            hallway_spots.insert({i, '.'});

        rooms = {
          { {'A', '.', '.', 2},
            {'B', '.', '.', 4},
            {'C', '.', '.', 6},
            {'D', '.', '.', 8} }
        };

        rooms[0].front = fish[0];
        rooms[1].front = fish[1];
        rooms[2].front = fish[2];
        rooms[3].front = fish[3];

        rooms[0].back = fish[4];
        rooms[1].back = fish[5];
        rooms[2].back = fish[6];
        rooms[3].back = fish[7];
    }

    friend std::strong_ordering operator<=>(const State& lhs, const State& rhs) {
        if (lhs.hallway_spots != rhs.hallway_spots)
            return (lhs.hallway_spots < rhs.hallway_spots) 
                ? std::strong_ordering::less 
                : std::strong_ordering::greater;
        for (int i {0}; i < 4; i++) {
            if (lhs.rooms[i].front != rhs.rooms[i].front)
                return lhs.rooms[i].front <=> rhs.rooms[i].front;

            if (lhs.rooms[i].back != rhs.rooms[i].back)
                return lhs.rooms[i].back <=> rhs.rooms[i].back;
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
            out << state.rooms[i].front << '#';
        out << "##\n";

        out << "  #";
        for (int i {0}; i < 4; i++)
            out << state.rooms[i].back << '#';
        out << '\n';

        return out << "  #########\n";
    }

    bool is_done() const {
        return rooms[0].front == 'A' && rooms[0].back == 'A' &&
               rooms[1].front == 'B' && rooms[1].back == 'B' &&
               rooms[2].front == 'C' && rooms[2].back == 'C' &&
               rooms[3].front == 'D' && rooms[3].back == 'D';
    }

    /* Returns if the fish can go home unblocked. From (x, 0) */
    std::pair<bool, int> can_go_home(char fish, const int x0) const {
        /* false if home is not open. */
        const Room& room {rooms[fish - 'A']};
        if (room.front != '.' || (room.back != '.' && room.back != fish))
            return {false, -123456789};

        int x_final {room.x_loc};
        int dx {(x_final - x0 > 0) ? 1 : -1};

        for (int x {x0 + dx}; x != x_final; x += dx) {
            if (!hallway_spots.contains(x))
                continue;
            if (hallway_spots.at(x) != '.')
                return {false, -123456789};
        }

        int cost = energies[fish - 'A'] * std::abs(x_final - x0);
        if (room.back != '.')
            cost += energies[fish - 'A'];
        else
            cost += 2 * energies[fish - 'A'];

        return {true, cost};
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

            auto [going_home, cost] {can_go_home(ch, x_pos)};
            if (going_home) {
                State next_state {*this};

                next_state.hallway_spots.at(x_pos) = '.';

                if (next_state.rooms[ch - 'A'].back == '.')
                    next_state.rooms[ch - 'A'].back = ch;
                else
                    next_state.rooms[ch - 'A'].front = ch;

                output.push_back({next_state, cost});
            }
        }

        /* For fish in the rooms, try to move them to their home rooms. */
        for (int room_index {0}; room_index < 4; room_index++) {
            const Room& room = rooms[room_index];

            for (auto [ch, y] : std::array<std::pair<char, int>, 2> {{{room.front, 1}, {room.back, 2}}}) {
                if (ch == '.')
                    continue; // Nothing here.
                if (ch - 'A' == room_index && y == 2)
                    continue; // Fish is home.
                if (ch - 'A' == room_index && room.back == ch)
                    continue; // Fish is home (front). Back is also home.
                if (y == 2 && room.front != '.')
                    continue; // Fish cannot move out.

                auto [going_home, home_cost] {can_go_home(ch, room.x_loc)};
                if (going_home) {
                    State next_state {*this};
                    home_cost += energies[ch - 'A'] * y; // Cost to move out of room.

                    if (y == 1)
                        next_state.rooms[room_index].front = '.';
                    else
                        next_state.rooms[room_index].back = '.';

                    if (next_state.rooms[ch - 'A'].back == '.')
                        next_state.rooms[ch - 'A'].back = ch;
                    else
                        next_state.rooms[ch - 'A'].front = ch;

                    output.push_back({next_state, home_cost});
                }

                /* Otherwise attempt moves into the hallway */
                else {
                    std::vector<int> moves {hallway_moves((room_index + 1) * 2)};
                    for (int x_final : moves) {
                        State next_state {*this};
                        int hallway_cost {energies[ch - 'A'] * y}; // Cost to move out of room
                        hallway_cost += energies[ch - 'A'] * std::abs(((room_index + 1) * 2) - x_final);

                        if (y == 1)
                            next_state.rooms[room_index].front = '.';
                        else
                            next_state.rooms[room_index].back = '.';

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
    std::array<char, 8> input;
    std::cin >> str 
             >> str
             >> ch >> ch >> ch >> input[0] >> ch >> input[1] >> ch >> input[2] >> ch >> input[3] >> str
             >> ch >> input[4] >> ch >> input[5] >> ch >> input[6] >> ch >> input[7];

    State state {};
    state.init(input);

    std::cout << find_lowest_cost_from(state) << '\n';

    return 0;
}
