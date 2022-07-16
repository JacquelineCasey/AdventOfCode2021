
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

/* The trick is to use a stack. */

/* This error type is a little hacky, since only the offending_char or the completion_string
 * is meant to be used at a given time. However, it gets the job done. */
/* Use a std::variant<> or polymorphism if you want something better. A union, maybe? */
struct ErrorInfo {
    enum {INCOMPLETE, CORRUPTED} error_type {};
    char offending_char {};
    std::string completion_string {};
};

char left_to_right(char left) {
    switch (left) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    case '<': return '>';
    default:
        assert(false && "Unrecognized character in left_to_right()");
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
        case '>': { // apparently compiler is upset if you declare variables in switch, if not in an explicity block
            if (open_chunks.empty())
                return {ErrorInfo::CORRUPTED, ch, ""};
            
            char matching {open_chunks.top()};
            open_chunks.pop();
            if (left_to_right(matching) != ch)
                return {ErrorInfo::CORRUPTED, ch, ""};

            break;
        }
        default:
            assert(false && "Unrecognized character in input string.");
        }
    }

    assert(!open_chunks.empty() && "Unexpected working input.");

    /* Now complete the string */
    std::string completion_str {};
    while (!open_chunks.empty()) {
        char right {open_chunks.top()};
        open_chunks.pop();

        completion_str += left_to_right(right);
    }

    return {ErrorInfo::INCOMPLETE, '?', completion_str};
}

const std::map<char, int> char_scores {
    {')', 1},
    {']', 2},
    {'}', 3},
    {'>', 4}
};

int main() {
    std::string line {};
    std::vector<long> scores {}; // Another gotcha with needing a longs instead of ints.

    while (std::cin >> line) {
        ErrorInfo e {parse_line(line)};
        if (e.error_type == ErrorInfo::INCOMPLETE) {
            long score {0};
            for (char c : e.completion_string) {
                score *= 5;
                score += char_scores.at(c);
            }
            scores.push_back(score);
        }
    }

    std::sort(scores.begin(), scores.end());
    std::cout << "Middle Score: " << scores[scores.size() / 2] << '\n';

    return 0;
}