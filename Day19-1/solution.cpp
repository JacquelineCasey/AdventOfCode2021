
#include <algorithm>
#include <cassert>
#include <compare>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <vector>


struct Triple {
public:
    int x {};
    int y {};
    int z {};

    friend std::istream& operator>>(std::istream& in, Triple& triple);

    friend std::strong_ordering operator<=>(const Triple& lhs, const Triple& rhs) = default;

    friend Triple operator-(const Triple& a, const Triple& b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    friend Triple operator+(const Triple& a, const Triple& b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    friend std::ostream& operator<<(std::ostream& out, const Triple& t) {
        return out << '{' << t.x << ", " << t.y << ", " << t.z << '}'; 
    }
};

class Rotation {
public:
    enum class Axis {
        X,
        Y,
        Z
    };

private:
    std::vector<std::pair<Axis, Axis>> steps {};

    int& select_axis(Axis ax, Triple& t) const {
        if (ax == Axis::X)
            return t.x;
        if (ax == Axis::Y)
            return t.y;
        if (ax == Axis::Z)
            return t.z;

        assert(false && "Should not reach this point");
    }

public:
    /* This could be smarter, removing instructions if some reverse or compose etc. */
    Rotation& add_step(Axis from, Axis to) {
        if (from == to)
            return *this;

        steps.push_back({from, to});

        return *this;
    }

    Triple rotate(Triple t) const {
        for (auto [from, to] : steps) {
            int& from_i {select_axis(from, t)};
            int& to_i {select_axis(to, t)};

            std::swap(from_i, to_i);
            from_i *= -1;
        }

        return t;
    }

    Rotation reverse() const {
        Rotation rev {};

        // The order of the steps are reversed.
        for (auto it {steps.rbegin()}; it != steps.rend(); it++) {
            auto [from, to] = *it;
            rev.add_step(to, from); // The steps are reversed internally.
        }

        return rev;
    }

    friend Rotation operator+(const Rotation& a, const Rotation& b) {
        Rotation rot {};
        
        for (auto [from, to] : a.steps) 
            rot.add_step(from, to);

        for (auto [from, to] : b.steps) 
            rot.add_step(from, to);

        return rot;
    }
};

class Scanner {
public:
    struct DisplacementInfo {
        /* x stores the smallest difference (positive), y stores the second
         * smallest difference (positive), and z stores the largest diference 
         * (positive or negative). rotation_from_original represents the rotation 
         * that take (b - a) to rotated_displacement. 
         * 
         * This allows us to sort displacements, and quickly find equivalent 
         * displacements between two rotated scanners. */
        Triple rotated_displacement {};
        Rotation rotation_from_original {};

        const int index_a {}; // Index of the origin in scanner.
        const int index_b {}; // Index of the endpoint in the scanner.

        DisplacementInfo(int a, int b, const std::vector<Triple>& triples) :
                index_a {a}, index_b {b} {

            const Triple d {triples[a] - triples[b]}; // Displacement Vector

            /* Rotate smallest displacement into the x position. */
            if (std::abs(d.y) <= std::abs(d.x) && std::abs(d.y) <= std::abs(d.z))
                rotation_from_original.add_step(Rotation::Axis::Y, Rotation::Axis::X);
            else if (std::abs(d.z) <= std::abs(d.x) && std::abs(d.z) <= std::abs(d.y))
                rotation_from_original.add_step(Rotation::Axis::Z, Rotation::Axis::X);

            rotated_displacement = rotation_from_original.rotate(d);
            
            /* Make it positive. */
            if (rotated_displacement.x < 0) {
                rotation_from_original.add_step(Rotation::Axis::X, Rotation::Axis::Y)
                                      .add_step(Rotation::Axis::X, Rotation::Axis::Y);
                rotated_displacement = rotation_from_original.rotate(d);
            }

            /* Rotate the next smallest displacement into the y position. */
            if (std::abs(rotated_displacement.z) <= std::abs(rotated_displacement.y)) {
                rotation_from_original.add_step(Rotation::Axis::Z, Rotation::Axis::Y);
                rotated_displacement = rotation_from_original.rotate(d);
            }

            /* Make it positive. */
            if (rotated_displacement.y < 0) {
                rotation_from_original.add_step(Rotation::Axis::Z, Rotation::Axis::Y).add_step(Rotation::Axis::Z, Rotation::Axis::Y);
                rotated_displacement = rotation_from_original.rotate(d);
            }

            /* Edge case: z is negative, but |z| = y. 
             * These need to have matching displacements, so make z positive. */
            if (rotated_displacement.z < 0 && std::abs(rotated_displacement.z) == rotated_displacement.y) {
                rotation_from_original.add_step(Rotation::Axis::Y, Rotation::Axis::Z);
                rotated_displacement = rotation_from_original.rotate(d);
            }

            /* Edge case: z is negative, but x is 0. 
             * These need to have matching displacements, so make z positive. */
            if (rotated_displacement.z < 0 && rotated_displacement.x == 0) {
                rotation_from_original.add_step(Rotation::Axis::X, Rotation::Axis::Z).add_step(Rotation::Axis::X, Rotation::Axis::Z);
                rotated_displacement = rotation_from_original.rotate(d);
            }

            assert(rotated_displacement.x >= 0 && rotated_displacement.y >= 0 &&
                   rotated_displacement.x <= rotated_displacement.y &&
                   rotated_displacement.y <= std::abs(rotated_displacement.z));
        }


        friend std::strong_ordering operator<=>(const DisplacementInfo& lhs, const DisplacementInfo& rhs) {
            return lhs.rotated_displacement <=> rhs.rotated_displacement;
        }
    };

private:
    std::multiset<DisplacementInfo> displacements {}; // Maps displacement heuristic to info.

public:
    std::vector<Triple> original_triples {};

    void construct_rotated_displacements() {
        /* Construct one displacement per each ordered pair (so xy and yx). They
         * do end up with different rotated_displacements. */
        for (int i {0}; i < static_cast<int>(original_triples.size()); i++) {
            for (int j {0}; j < static_cast<int>(original_triples.size()); j++) {
                if (i == j) 
                    continue;
                displacements.insert({DisplacementInfo(i, j, original_triples)});
            }
        }
    }

    std::optional<std::pair<Triple, Rotation>> attempt_connect(Scanner& other) {
        std::multiset<DisplacementInfo> intersection {}; // Must allow for multiple elements.

        /* Copies items from the first iterator, if equivalent items exist in the
         * second iterator. */
        std::set_intersection(displacements.begin(), displacements.end(), 
                              other.displacements.begin(), other.displacements.end(),
                              std::inserter(intersection, intersection.end()));

        /* If 12 common probes exist, there will be (at least) 12 * 11 common displacements. */
        if (intersection.size() < 132) 
            return std::nullopt;
        
        /* This might not be truly rock solid, but I bet it will work. */
        for (const DisplacementInfo& info : intersection) {
            const DisplacementInfo& other_info {*other.displacements.find(info)};
            
            /* Transform our points by rotating according to info. Transform 
             * the other points by rotating by their info, and moving their 
             * info.index_a to where ours is. Look for 12 matches. */
            std::vector<Triple> transformed {};
            for (Triple t : original_triples) {
                transformed.push_back(info.rotation_from_original.rotate(t));
            }

            std::vector<Triple> other_transformed {};
            for (Triple t : other.original_triples) {
                other_transformed.push_back(other_info.rotation_from_original.rotate(t));
            }

            Triple offset = other_transformed[other_info.index_a] - transformed[info.index_a];
            for (Triple& t: other_transformed) {
                t = t - offset;
            }

            std::ranges::sort(transformed);
            std::ranges::sort(other_transformed);

            std::vector<Triple> common_points {};
            std::set_intersection(transformed.begin(), transformed.end(),
                                  other_transformed.begin(), other_transformed.end(),
                                  std::back_inserter(common_points));

            if (common_points.size() >= 12) {
                /* Now calculate information on the position of the other scanner itself. */
                Rotation r_other {other_info.rotation_from_original + info.rotation_from_original.reverse()};
                Triple pos {Triple {0, 0, 0} - info.rotation_from_original.reverse().rotate(offset)};

                return {{pos, r_other}}; 
            }
        }

        return std::nullopt;
    }

    friend std::istream& operator>>(std::istream& in, Scanner& scanner);
};

class ScannerMap {
private:
    std::vector<Scanner> scanners {};
    std::vector<std::optional<Triple>> scanner_locations {};
    std::vector<Rotation> scanner_rotations {};

public:
    void construct_map() {
        for (Scanner& sc : scanners) {
            sc.construct_rotated_displacements();
        }

        scanner_locations = std::vector<std::optional<Triple>>(scanners.size(), std::nullopt);
        scanner_locations[0] = {0, 0, 0}; // Declare scanner 0 to be the origin.

        scanner_rotations = std::vector<Rotation>(scanners.size());
        scanner_rotations[0] = {};

        std::queue<int> known_scanners {};
        known_scanners.push(0);

        while (!known_scanners.empty()) {
            int known {known_scanners.front()};
            known_scanners.pop();

            for (int i {0}; i < static_cast<int>(scanners.size()); i++) {
                if (scanner_locations[i].has_value())
                    continue;

                std::optional<std::pair<Triple, Rotation>> result {scanners[known].attempt_connect(scanners[i])};
                if (result.has_value()) {
                    auto [loc, rot] = result.value();
                    scanner_locations[i] = scanner_locations[known].value() + scanner_rotations[known].rotate(loc);

                    /* Performing this rotation will give the coordinates in terms of scanner 0. */
                    scanner_rotations[i] = rot + scanner_rotations[known]; 

                    known_scanners.push(i);
                }
            }
        }

        std::cout << "Map fully built" << '\n';
    }

    /* Assumes constructed map. */
    int count_nodes() {
        std::set<Triple> beacons {};

        for (int i {0}; i < static_cast<int>(scanners.size()); i++) {
            for (const Triple& t: scanners[i].original_triples) {
                beacons.insert(
                    scanner_rotations[i].rotate(t) + scanner_locations[i].value()
                );
            }
        }
        
        return beacons.size();
    }

    friend std::istream& operator>>(std::istream& in, ScannerMap& scanner_map);
};

/* Parsing Input */

std::istream& operator>>(std::istream& in, Triple& triple) {
    char buf {};
    in >> triple.x >> buf >> triple.y >> buf >> triple.z;
    in.ignore(); // Clear the newline as well.

    return in;
}

std::istream& operator>>(std::istream& in, Scanner& scanner) {
    scanner = {};
    std::string buf {};
    std::getline(in, buf); // --- scanner # ---

    while (std::getline(in, buf)) {
        if (buf.size() == 0)
            break;

        scanner.original_triples.push_back({});
        std::istringstream {buf} >> scanner.original_triples.back();
    }

    return in;
}

std::istream& operator>>(std::istream& in, ScannerMap& scanner_map) {
    scanner_map = {};
    Scanner sc {};

    while (in >> sc) {
        scanner_map.scanners.push_back(sc);   
    }

    scanner_map.scanners.push_back(sc); // Due to lack of white space, last scanner cause cin to fail.

    return in;
}

/* Main */

int main() {
    ScannerMap sm {};
    std::cin >> sm;

    sm.construct_map();

    std::cout << sm.count_nodes() << '\n';

    return 0;
}
