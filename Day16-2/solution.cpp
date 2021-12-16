
#include <array>
#include <cassert>
#include <climits>
#include <iostream>
#include <map>
#include <variant>
#include <vector>

/* Once again, you have to use a long to fit the number we create. */

struct Packet {
public:
    enum class Type {
        Sum = 0,
        Product = 1,
        Minimum = 2,
        Maximum = 3,
        Literal = 4,
        Greater_Than = 5,
        Less_Than = 6,
        Equal_To = 7
    };

    int version {};
    Type type {};

    std::variant<long, std::vector<Packet>> payload {};

    long evaluate() const {
        switch (type) {
        case Type::Sum: {
            long sum {0};
            for (const Packet& subpacket : std::get<std::vector<Packet>>(payload))
                sum += subpacket.evaluate();
            return sum;
        }
        case Type::Product: {
            long product {1};
            for (const Packet& subpacket : std::get<std::vector<Packet>>(payload))
                product *= subpacket.evaluate();
            return product;
        }
        case Type::Minimum: {
            long min {LONG_MAX};
            for (const Packet& subpacket : std::get<std::vector<Packet>>(payload))
                min = std::min(min, subpacket.evaluate());
            return min;
        }
        case Type::Maximum: {
            long max {LONG_MIN};
            for (const Packet& subpacket : std::get<std::vector<Packet>>(payload))
                max = std::max(max, subpacket.evaluate());
            return max;
        }
        case Type::Literal: {
            return std::get<long>(payload);
        }
        case Type::Greater_Than: {
            auto subpackets {std::get<std::vector<Packet>>(payload)};
            return subpackets[0].evaluate() > subpackets[1].evaluate();
        }
        case Type::Less_Than: {
            auto subpackets {std::get<std::vector<Packet>>(payload)};
            return subpackets[0].evaluate() < subpackets[1].evaluate();
        }
        case Type::Equal_To: {
            auto subpackets {std::get<std::vector<Packet>>(payload)};
            return subpackets[0].evaluate() == subpackets[1].evaluate();
        }
        }

        assert(false && "Should not reach the end of this switch");
    }
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

        packet.type = static_cast<Packet::Type>(read_int(3));

        if (packet.type == Packet::Type::Literal)
            parse_literal(packet);
        else
            parse_operator(packet);

        return packet;
    }

private:
    void parse_literal(Packet& packet) {
        long num {0};

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
        long length_type {read_int(1)};

        if (length_type == 0) {
            long total_length {read_int(15)};
            int start_index {index};
            while (index < start_index + total_length) {
                subpackets.push_back(parse_packet());
            }
        }
        else {
            long num_packets {read_int(11)};
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

    long read_int(int n_bits) {
        long num {0};

        for (int i {0}; i < n_bits; i++) {
            num *= 2;
            num += bits.at(index);
            index++;
        }

        return num;
    }
};

int main() {
    std::string hex {};
    std::cin >> hex;

    Packet packet = Parser {hex}.parse_packet();

    std::cout << packet.evaluate() << '\n';
    return 0;
}
