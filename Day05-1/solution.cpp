
#include <cassert>
#include <map> // n.b. unordered_map would be algorithmically better, but cannot hash structs without us providing our own hash function
#include <iostream>

/* Let's learn how to make iterators! 
 * In C++20, concepts are used for iterators, so to make something iterable, we
 * simply give it begin() and end() functions. 
 * These need the return an iterator type, with operators ++, !=, and * provided.
 * C++ has a bunch of types of iterators, and it seems the system can deduce what
 * type ours is. You can only go forwards (right now), and you can only go through
 * a given iterator once. */

struct Point2D {
    int x {};
    int y {};
}; 

class Line {
private:
    Point2D p1;
    Point2D p2;
    class Iterator;

public:
    friend std::istream& operator>>(std::istream& in, Line& line);

    bool isVerticalOrHorizontal() {
        return p1.x == p2.x || p1.y == p2.y;
    }

    Iterator begin() {
        return Iterator(*this);
    }

    Iterator end() {
        return Iterator::sentinal(*this);
    }

private:
    class Iterator {
    public:
        Iterator(const Line& line) : 
            // line {line}, 
            step {get_step(line)},
            current {line.p1}
        {}; 

        /* Returns a one past the end iterator. */
        static Iterator sentinal(const Line& line) {
            Iterator ret = Iterator(line);
            ret.current = line.p2;
            ret.current.x += ret.step.x;
            ret.current.y += ret.step.y;

            return ret;
        }

        /* Preincrement */
        Iterator& operator++() {
            current.x += step.x;
            current.y += step.y;

            return *this;
        }

        /* Postincrement */
        Iterator operator++(int) {
            Iterator temp {*this};
            ++*this;
            return temp;
        }

        Point2D operator*() const {
            return current;
        }

        bool operator!=(const Iterator& other) const {
            return current.x != other.current.x || current.y != other.current.y;
        }

    private:
        Point2D get_step(const Line& line) {
            if (line.p1.x == line.p2.x) { // Horizontal
                if (line.p1.y < line.p2.y)
                    return {0, 1};
                return {0, -1};
            }
            if (line.p1.y == line.p2.y) { // Vertical
                if (line.p1.x < line.p2.x)
                    return {1, 0};
                return {-1, 0};
            }
            
            std::cout << "\n Error in get_step(): non horizontal or vertical lines not supported \n"; 
            assert(false);
        }


        // const Line& line;

        const Point2D step;
        Point2D current;
    };
};

std::istream& operator>>(std::istream& in, Line& line) {
    line = {};
    char buffer_char {};
    std::string buffer_string {};

    in >> line.p1.x;
    in >> buffer_char; // ','
    in >> line.p1.y;
    
    in >> buffer_string; // " --> "

    in >> line.p2.x;
    in >> buffer_char; // ','
    in >> line.p2.y;

    return in;
}


int main() {
    std::map<std::pair<int, int>, int> map {};

    Line curr {};
    while (std::cin >> curr) {
        if (!curr.isVerticalOrHorizontal())
            continue;
        for (Point2D p : curr) {
            map[std::pair(p.x, p.y)]++;
        }
    }

    int count {};
    for (auto [point, vents] : map) {
        if (vents > 1)
            count++;
    }

    std::cout << count << std::endl;
}
