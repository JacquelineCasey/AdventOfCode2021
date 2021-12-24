
/* Observe that every time the input is read, x and y are zeroed before they 
 * are used. (w is effectively zeroed as well). We can therefore split by 
 * section, and attempt to memoize: What input (if anything) is known to work for
 * certain z at this time step. */

/* If need be, we can also use another property of the input. Each segment may
 * increase z, but z is only every decreased by div z 26. Since z must eventaully
 * become zero, we can prune many calculations where z explodes. 1 division left
 * means that z must be below 26. 2 divisions left imply 26^2 is the upper bound. */

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <vector>

struct Registers {
    __int128_t w {0};
    __int128_t x {0};
    __int128_t y {0};
    __int128_t z {0};
};

struct Instruction {
    std::string op {};
    std::string arg1 {};
    std::string arg2 {};

    Registers run(Registers registers, std::queue<int>& input) const {
        __int128_t constant {-40000};
        __int128_t* dst {select(arg1, registers)};
        __int128_t* src {select(arg2, registers)};

        assert(dst != nullptr && "arg1 must refer to a register (not a constant).");
        if (src == nullptr && op != "inp") {
            constant = std::stoi(arg2);
            src = &constant;
        }

        if (op == "inp") {
            *dst = input.front();
            input.pop();
        }
        else if (op == "add")
            *dst += *src;
        else if (op == "mul")
            *dst *= *src;
        else if (op == "div")
            *dst /= *src;
        else if (op == "mod")
            *dst %= *src;
        else if (op == "eql")
            *dst = *dst == *src;

        return registers;
    }

    friend std::istream& operator>>(std::istream& in, Instruction& instruction) {
        in >> instruction.op >> instruction.arg1;
        if (instruction.op == "inp")
            return in;
        else
            return in >> instruction.arg2;
    }

    friend bool operator==(const Instruction& lhs, const Instruction& rhs) {
        return (lhs.op == rhs.op && lhs.arg1 == rhs.arg1 && lhs.arg2 == rhs.arg2);
    }

private:
    /* Returns a pointer to one of the registers, or null (which implies a constant) */
    __int128_t* select(std::string arg, Registers& registers) const {
        switch (arg[0]) {
        case 'w': return &registers.w;
        case 'x': return &registers.x;
        case 'y': return &registers.y;
        case 'z': return &registers.z;
        default: return nullptr;
        }
    }
};

struct Segment {
    std::vector<Instruction> instructions {};

    /* Only ever one int input. */
    Registers run(Registers registers, int input) const {
        std::queue<int> queue {};
        queue.push(input);

        for (const Instruction& instr : instructions) {
            registers = instr.run(registers, queue);
        }

        return registers;
    }

    friend std::istream& operator>>(std::istream& in, Segment& segment) {
        /* Each segment has 18 instructions. */
        for (int i {0}; i < 18; i++) {
            segment.instructions.push_back({});
            in >> segment.instructions.back();
        }

        return in;
    }
};

struct SegmentedProgram {
    /* Each segment starts with inp w. */
    std::array<Segment, 14> segments {};
    /* If, before the execution of segment[i], z >= z_bounds[i], we know z will
     * never be 0. */
    std::array<__int128_t, 14> z_bounds {};

    void prepare_z_bounds() {
        __int128_t curr_bound {1};
        for (int i {13}; i >= 0; i--) {
            /* Always the 5th (index = 4) instruction. */
            if (segments[i].instructions[4] == Instruction {"div", "z", "26"})
                curr_bound *= 26;
            z_bounds[i] = curr_bound;
        }
    }

    friend std::istream& operator>>(std::istream& in, SegmentedProgram& program) {
        /* Each program has 14 segments. */
        for (int i {0}; i < 14; i++) {
            in >> program.segments[i];
        }

        return in;
    }
};

/* This time, we only care about z. */
__int128_t run_segment(const Segment& segment, int input, __int128_t z) {
    auto [w, x, y, z_new] {segment.run({0,0,0,z}, input)};
    return z_new;
} 

/* memo_map_steps[n] maps the z values at that step to the 1-9 input that corresponds
 * to the step, for which the program may eventually output z = 0, ie VALID. 
 * For example, memo_map_steps[3][17] returns all of the digits at position 3 for
 * which there exists some possible successful remaining input at z = 17.
 * That input could be found by performing program[3] on the input for one of
 * the digits, and looking up memo_map_steps[4][next_z]. Of course, generating
 * all such numbers this way is expensive. Instead, we intend to compute some
 * agregate info, like greatest valid number (part 1). */
std::array<std::map<__int128_t, std::set<int>>, 14> memo_map_steps {};

/* Returns whether this z value, at this step, succeeds for some input (which is
 * placed in memo_map_steps)*/
bool has_valid_tail_from(const SegmentedProgram& program, const int step = 0, const __int128_t z = 0) {
    /* If we have reached the end of the program: */
    if (step == 14)
        return z == 0; // Return whether the input was valid.

    /* If we know z will never be zero: */
    if (z >= program.z_bounds[step])
        return false;
    
    /* If we have computed this before: */
    if (memo_map_steps[step].contains(z))
        return memo_map_steps[step][z].size() > 0; // Return whether there is a valid possible input tail.

    /* Perform the computation, and build memo map. */
    memo_map_steps[step].insert({z, {}});
    for (int i {1}; i <= 9; i++)
        if (has_valid_tail_from(program, step + 1, run_segment(program.segments[step], i, z)))
            memo_map_steps[step][z].insert(i);

    return memo_map_steps[step][z].size() > 0;    
}



long find_largest_from_step(const SegmentedProgram& program, const int step = 0, const __int128_t z = 0) {
    /* Last step */
    if (step == 13) {
        return *memo_map_steps[13][z].rbegin();
    }

    int this_digit {*memo_map_steps[step][z].rbegin()};
    long moved_digit = this_digit;
    for (int i {step}; i < 13; i++)
        moved_digit *= 10;
    
    return moved_digit + find_largest_from_step(program, step + 1, run_segment(program.segments[step], this_digit, z));
}

int main() {
    SegmentedProgram prog {};
    std::cin >> prog;
    prog.prepare_z_bounds();

    has_valid_tail_from(prog, 0, 0);

    std::cout << find_largest_from_step(prog) << '\n';

    return 0;
}