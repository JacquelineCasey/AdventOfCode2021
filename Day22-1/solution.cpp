
/* 
 * Here is the big picture:
 * - Conceptualize the entire space that we care about.
 * - Slice it up along every X boundary that could seperate any two instruction
 *   cubes (~ 2 * N_Instruction Slices)
 * - For each instruction, go to that instructions slice(s), and chop up along
 *   Y boundaries. (Same number of slices, but only a handful per preexisting slice).
 * - For each instruction, go to its region, and chop up by Z boundaries.
 * - Run the instructions. Find all the cuboid regions they affect, and set
 *   them on or off as a whole. This scheme ensures that they are as split as
 *   they need to be.
 * - For each cuboid that is on, determining its size via multiplication, and 
 *   add up those sizes.
 * */

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

struct Instruction {
    bool state {false};
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;
};

std::istream& operator>>(std::istream& in, Instruction& instruction) {
    char ch {};
    std::string str {};

    in >> str;
    instruction.state = str == "on";

    in >> ch >> ch >> instruction.x1 >> ch >> ch >> instruction.x2; // x=___..___
    in >> ch; // ','
    in >> ch >> ch >> instruction.y1 >> ch >> ch >> instruction.y2; // y=___..___
    in >> ch; // ','
    in >> ch >> ch >> instruction.z1 >> ch >> ch >> instruction.z2; // z=___..___
    
    return in;
}

struct Region {
    int z_min {};
    int z_max {};
    bool on {false};
};

struct YSlice {
    int y_min {};
    int y_max {};
    std::map<int, Region> slices {}; // Maps z_min to slice with that z_min;
    std::vector<int> z_low_bounds {};
};

struct XSlice {
    int x_min {};
    int x_max {};
    std::map<int, YSlice> slices {}; // Mpas y_min to slice with that y_min;
    std::vector<int> y_low_bounds {};
};

struct Grid {
    std::map<int, XSlice> slices {}; // Maps x_min to slice with that x_min.
};

void slice_x_bounds(Grid& grid, const std::vector<Instruction> instructions) {
    std::vector<int> slice_left_bounds {};
    for (const Instruction& instruction : instructions) {
        slice_left_bounds.push_back(instruction.x1);
        slice_left_bounds.push_back(instruction.x2 + 1); // Left bound of right neighbor.
    }
    std::ranges::sort(slice_left_bounds);

    for (int i {0}; i < static_cast<int>(slice_left_bounds.size() - 1); i++) {
        XSlice curr {};
        curr.x_min = slice_left_bounds[i];
        curr.x_max = slice_left_bounds[i + 1] - 1;

        if (curr.x_max < curr.x_min)
            continue; // 0 width slices can be ignored

        grid.slices.insert({curr.x_min, curr});
    }
}

void slice_y_bounds(Grid& grid, const std::vector<Instruction> instructions) {
    for (const Instruction& instruction : instructions) {
        int curr_left_x {instruction.x1};
        while (curr_left_x <= instruction.x2 && grid.slices.contains(curr_left_x)) {
            XSlice& slice {grid.slices.at(curr_left_x)};
            slice.y_low_bounds.push_back(instruction.y1);
            slice.y_low_bounds.push_back(instruction.y2 + 1); // Low bound of top neighbor.

            curr_left_x = slice.x_max + 1; // This is how we access the next slice.
        }
    }

    /* Perform the slices from those vectors. */
    for (auto& [_, x_slice] : grid.slices) {
        std::ranges::sort(x_slice.y_low_bounds);

        for (int i {0}; i < static_cast<int>(x_slice.y_low_bounds.size() - 1); i++) {
            YSlice curr {};
            curr.y_min = x_slice.y_low_bounds[i];
            curr.y_max = x_slice.y_low_bounds[i + 1] - 1;

            if (curr.y_max < curr.y_min)
                continue; // 0 width slices can be ignored

            x_slice.slices.insert({curr.y_min, curr});
        }
    }
}

void slice_z_bounds(Grid& grid, const std::vector<Instruction> instructions) {
    for (const Instruction& instruction : instructions) {
        int curr_left_x {instruction.x1};
        while (curr_left_x <= instruction.x2 && grid.slices.contains(curr_left_x)) {
            XSlice& x_slice {grid.slices.at(curr_left_x)};
            int curr_low_y {instruction.y1};
            while (curr_low_y <= instruction.y2 && x_slice.slices.contains(curr_low_y)) {
                YSlice& y_slice {x_slice.slices.at(curr_low_y)};
                y_slice.z_low_bounds.push_back(instruction.z1);
                y_slice.z_low_bounds.push_back(instruction.z2 + 1); // back bound of front neighbor.

                curr_low_y = y_slice.y_max + 1;
            }

            curr_left_x = x_slice.x_max + 1;
        }
    }

    /* Perform the slices from those vectors. */
    for (auto& [_, x_slice] : grid.slices) {
        for (auto& [_, y_slice] : x_slice.slices) {
            std::ranges::sort(y_slice.z_low_bounds);

            for (int i {0}; i < static_cast<int>(y_slice.z_low_bounds.size() - 1); i++) {
                Region curr {};
                curr.z_min = y_slice.z_low_bounds[i];
                curr.z_max = y_slice.z_low_bounds[i + 1] - 1;

                if (curr.z_max < curr.z_min)
                    continue; // 0 width slices can be ignored

                y_slice.slices.insert({curr.z_min, curr});
            }
        }
    }
}

void run_instructions(Grid& grid, const std::vector<Instruction> instructions) {
    for (const Instruction& instruction : instructions) {

        int curr_left_x {instruction.x1};
        while (curr_left_x <= instruction.x2 && grid.slices.contains(curr_left_x)) {
            XSlice& x_slice {grid.slices.at(curr_left_x)};

            int curr_low_y {instruction.y1};
            while (curr_low_y <= instruction.y2 && x_slice.slices.contains(curr_low_y)) {
                YSlice& y_slice {x_slice.slices.at(curr_low_y)};

                int curr_low_z {instruction.z1};
                while (curr_low_z <= instruction.z2 && y_slice.slices.contains(curr_low_z)) {
                    Region& region {y_slice.slices.at(curr_low_z)};
                    region.on = instruction.state;

                    curr_low_z = region.z_max + 1;
                }

                curr_low_y = y_slice.y_max + 1;
            }

            curr_left_x = x_slice.x_max + 1;
        }
    }
}

long count_on(const Grid& grid) {
    long count {0};
    for (const auto& [_, x_slice] : grid.slices) {
        for (const auto& [_, y_slice] : x_slice.slices) {
            for (const auto& [_, region] : y_slice.slices) {
                if (region.on) {
                    long volume {
                        (x_slice.x_max - x_slice.x_min + 1) *
                        (y_slice.y_max - y_slice.y_min + 1) *
                        (region.z_max - region.z_min + 1)
                    };
                    
                    count += volume;
                }
            }
        }
    }

    return count;
}

int main() {
    std::vector<Instruction> instructions {};
    Instruction instruction;
    while (std::cin >> instruction) {
        /* Ignore instructions that don't impact the Part1 Region. */
        if (instruction.x1 > 50 || instruction.x2 < -50 ||
            instruction.y1 > 50 || instruction.y2 < -50 ||
            instruction.z1 > 50 || instruction.z2 < -50)
            continue;

        instructions.push_back(instruction);
    }

    Grid grid {};
    slice_x_bounds(grid, instructions);
    slice_y_bounds(grid, instructions);
    slice_z_bounds(grid, instructions);
    run_instructions(grid, instructions);

    std::cout << count_on(grid) << '\n';

    return 0;
}