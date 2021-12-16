
#include <array>
#include <iostream>
#include <map>
#include <variant>
#include <vector>

struct Packet {
public:
    enum class Type {
        Operator,
        Literal
    };

    int version {};
    Type type {};

    std::variant<int, std::vector<Packet>> payload {};
};

class Parser {
private:
    const std::vector<bool> bits;
    int index;

public:
    Parser(const std::string& hexadecimal) : 
        bits {from_hex(hexadecimal)}, index {0} 
    {}

    Packet parse_packet() {
        Packet packet {};

        packet.version = read_int(3);

        switch (read_int(3)) {
        case 4:
            packet.type = Packet::Type::Literal;
            break;
        default:
            packet.type = Packet::Type::Operator;
        }

        if (packet.type == Packet::Type::Literal)
            parse_literal(packet);
        else
            parse_operator(packet);

        return packet;
    }

private:
    void parse_literal(Packet& packet) {
        int num {0};

        while (true) {
            bool keep_going = read_int(1);

            for (int i {0}; i < 4; i++) {
                num *= 2;
                num += read_int(1);
            }

            if (!keep_going)
                break;
        } 

        packet.payload = num;
    }

    void parse_operator(Packet& packet) {
        std::vector<Packet> subpackets {};
        int length_type {read_int(1)};

        if (length_type == 0) {
            int total_length {read_int(15)};
            int start_index {index};
            while (index < start_index + total_length) {
                subpackets.push_back(parse_packet());
            }
        }
        else {
            int num_packets {read_int(11)};
            for (int i {0}; i < num_packets; i++) {
                subpackets.push_back(parse_packet());
            }
        }

        packet.payload = subpackets;
    }

    static std::vector<bool> from_hex(const std::string& hexadecimal) {
        static const std::map<char, std::array<bool, 4>> hex_map {
            {'0', {0, 0, 0, 0}},
            {'1', {0, 0, 0, 1}},
            {'2', {0, 0, 1, 0}},
            {'3', {0, 0, 1, 1}},
            {'4', {0, 1, 0, 0}},
            {'5', {0, 1, 0, 1}},
            {'6', {0, 1, 1, 0}},
            {'7', {0, 1, 1, 1}},
            {'8', {1, 0, 0, 0}},
            {'9', {1, 0, 0, 1}},
            {'A', {1, 0, 1, 0}},
            {'B', {1, 0, 1, 1}},
            {'C', {1, 1, 0, 0}},
            {'D', {1, 1, 0, 1}},
            {'E', {1, 1, 1, 0}},
            {'F', {1, 1, 1, 1}},
        };

        std::vector<bool> bits {};

        for (char c : hexadecimal)
            for (bool bit : hex_map.at(c))
                bits.push_back(bit);

        return bits;
    }

    int read_int(int n_bits) {
        int num {0};

        for (int i {0}; i < n_bits; i++) {
            num *= 2;
            num += bits.at(index);
            index++;
        }

        return num;
    }
};

int version_sum(const Packet& packet) {
    int sum {packet.version};
    
    if (packet.type == Packet::Type::Operator) {
        for (const Packet& subpacket : std::get<std::vector<Packet>>(packet.payload)) {
            sum += version_sum(subpacket);
        }
    }

    return sum;
}

int main() {
    std::string hex {};
    std::cin >> hex;

    Packet packet = Parser {hex}.parse_packet();

    std::cout << version_sum(packet) << '\n';
    return 0;
}
