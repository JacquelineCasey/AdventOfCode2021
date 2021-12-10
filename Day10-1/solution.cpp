
#include <cassert>
#include <iostream>
#include <map>
#include <stack>

/* The trick is to use a stack. */

struct ErrorInfo {
    enum {INCOMPLETE, CORRUPTED} error_type {};
    char offending_char {};
};

bool left_right_match(char left, char right) {
    switch (left) {
    case '(': return right == ')';
    case '[': return right == ']';
    case '{': return right == '}';
    case '<': return right == '>';
    default:
        assert(false && "Unrecognized character in left_right_match()");
    }
}

ErrorInfo parse_line(const std::string& line) {
    std::stack<char> open_chunks {};

    for (char ch : line) {
        switch (ch) {
        case '(':
        case '[':
        case '{':
        case '<':
            open_chunks.push(ch);
            break;
        case ')':
        case ']':
        case '}':
        case '>': {
            if (open_chunks.empty())
                return {ErrorInfo::CORRUPTED, ch};
            
            char matching {open_chunks.top()};
            open_chunks.pop();
            if (!left_right_match(matching, ch))
                return {ErrorInfo::CORRUPTED, ch};

            break;
        }
        default:
            assert(false && "Unrecognized character in input string.");
        }
    }

    assert(!open_chunks.empty() && "Unexpected working input.");
    return {ErrorInfo::INCOMPLETE, '?'};
}

const std::map<char, int> scores {
    {')', 3},
    {']', 57},
    {'}', 1197},
    {'>', 25137}
};

int main() {
    std::string line {};
    int total_score {};
    while (std::cin >> line) {
        ErrorInfo e {parse_line(line)};
        if (e.error_type == ErrorInfo::CORRUPTED) {
            total_score += scores.at(e.offending_char);
        }
    }

    std::cout << "Total Score: " << total_score << '\n';

    return 0;
}