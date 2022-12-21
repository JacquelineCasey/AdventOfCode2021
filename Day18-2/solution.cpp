
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <variant>
#include <vector>

class SnailfishNumber;

struct BranchNode {
public:
    std::unique_ptr<SnailfishNumber> left {nullptr};
    std::unique_ptr<SnailfishNumber> right {nullptr}; 
};

struct LeafNode {
public:
    int number {};
};

class SnailfishNumber {
private:
    enum class Position {
        Left = -1,
        Right = 1,
        Root = 2 // Reserved only for the root node.
    };

    std::variant<BranchNode, LeafNode> node {};
    SnailfishNumber* parent {nullptr}; // I couldn't figure out another way (I kinda need boost::recursive_wrapper)
    Position position {Position::Root};

    SnailfishNumber* get_left_neighbor() {
        SnailfishNumber* current {this};
       
        /* Move up until we have node on the right of its enclosing pair. */
        while (current->position == Position::Left) {
            current = current->parent;
        }

        if (current->position == Position::Root)
            return nullptr; // No left neighbor;

        /* Now get the current node's sibling (which will be on the left) */
        current = std::get<BranchNode>(current->parent->node).left.get();

        /* The rightmost child of this sibling is our left neighbor */
        while (std::holds_alternative<BranchNode>(current->node)) {
            current = std::get<BranchNode>(current->node).right.get();
        }
        
        return current;
    }

    SnailfishNumber* get_right_neighbor() {
        SnailfishNumber* current {this};
       
        /* Move up until we have node on the left of its enclosing pair. */
        while (current->position == Position::Right) {
            current = current->parent;
        }
        
        if (current->position == Position::Root)
            return nullptr; // No right neighbor;
         
        /* Now get the current node's sibling (which will be on the right) */
        current = std::get<BranchNode>(current->parent->node).right.get();

        /* The leftmost child of this sibling is our right neighbor */
        while (std::holds_alternative<BranchNode>(current->node)) {
            current = std::get<BranchNode>(current->node).left.get();  
        }       

        return current;
    }

    SnailfishNumber* findFourthLevelNestedPair(int nesting) {
        if (std::holds_alternative<LeafNode>(node))
            return nullptr;

        if (nesting == 4)
            return this;

        SnailfishNumber* ptr {std::get<BranchNode>(node).left->findFourthLevelNestedPair(nesting + 1)};
        if (ptr != nullptr)
            return ptr;

        return std::get<BranchNode>(node).right->findFourthLevelNestedPair(nesting + 1);
    }

    bool try_explode() {     
        SnailfishNumber* ptr {findFourthLevelNestedPair(0)};
        if (ptr == nullptr)
            return false;  

        BranchNode& bn {std::get<BranchNode>(ptr->node)};
        SnailfishNumber* left_ptr {ptr->get_left_neighbor()};
        SnailfishNumber* right_ptr {ptr->get_right_neighbor()}; 

        if (left_ptr != nullptr)
            std::get<LeafNode>(left_ptr->node).number += std::get<LeafNode>(bn.left->node).number;
        if (right_ptr != nullptr)
            std::get<LeafNode>(right_ptr->node).number += std::get<LeafNode>(bn.right->node).number;

        ptr->node = LeafNode {0};

        return true;
    }

    SnailfishNumber* findTenOrGreater() {
        if (std::holds_alternative<LeafNode>(node)) {
            return (std::get<LeafNode>(node).number >= 10) ? this : nullptr;
        }

        SnailfishNumber* ptr {std::get<BranchNode>(node).left->findTenOrGreater()};
        if (ptr != nullptr)
            return ptr;
        
        return std::get<BranchNode>(node).right->findTenOrGreater();
    }

    bool try_split() {
        SnailfishNumber* ptr {findTenOrGreater()};
        if (ptr == nullptr)
            return false;

        int big_number {std::get<LeafNode>(ptr->node).number};

        BranchNode new_bn {};
        new_bn.left = std::make_unique<SnailfishNumber>();
        new_bn.right = std::make_unique<SnailfishNumber>();

        new_bn.left->parent = ptr;
        new_bn.right->parent = ptr;
        new_bn.left->position = Position::Left;
        new_bn.right->position = Position::Right;
        new_bn.left->node = LeafNode({big_number / 2});
        new_bn.right->node = LeafNode({big_number / 2 + big_number % 2});

        ptr->node = std::move(new_bn);
        return true;
    }

public:
    SnailfishNumber() = default;

    SnailfishNumber(SnailfishNumber&& moved) {
        node = std::move(moved.node);
        position = moved.position;
        parent = moved.parent;

        /* If we get a new address, we must tell our children. */
        if (std::holds_alternative<BranchNode>(node)) {
            std::get<BranchNode>(node).left->parent = this;
            std::get<BranchNode>(node).right->parent = this;
        }
    } 

    friend std::istream& operator>>(std::istream& in, SnailfishNumber& num);

    static std::unique_ptr<SnailfishNumber> append(SnailfishNumber&& l, SnailfishNumber&& r) {
        std::unique_ptr<SnailfishNumber> result {std::make_unique<SnailfishNumber>()};
        BranchNode result_bn {};
        result_bn.left = std::make_unique<SnailfishNumber>(std::move(l));
        result_bn.right = std::make_unique<SnailfishNumber>(std::move(r));

        result_bn.left->parent = result.get();
        result_bn.right->parent = result.get();
        result_bn.left->position = SnailfishNumber::Position::Left;
        result_bn.right->position = SnailfishNumber::Position::Right;

        result->node = std::move(result_bn);

        // int step {0};
        while (true) {
            // step++;

            if (result->try_explode()) {
                continue;
            }

            if (result->try_split()) {
                continue;
            }

            break;
        }
        
        return result;
    }

    int magnitude() {
        if (std::holds_alternative<LeafNode>(node))
            return std::get<LeafNode>(node).number;

        BranchNode& bn {std::get<BranchNode>(node)};
        return 3 * bn.left->magnitude() + 2 * bn.right->magnitude();
    }
};

std::istream& operator>>(std::istream& in, SnailfishNumber& num) {
    num.parent = nullptr;
    num.position = SnailfishNumber::Position::Root;

    char buf {};
    in >> buf;
    
    if (buf >= '0' && buf <= '9') {
        num.node = LeafNode {static_cast<int>(buf - '0')};
        return in;
    }

    if (buf != '[') {
        in.setstate(std::ios::failbit);
        return in;
    }
    num.node = BranchNode {};
    std::get<BranchNode>(num.node).left = std::make_unique<SnailfishNumber>();
    std::get<BranchNode>(num.node).right = std::make_unique<SnailfishNumber>();
    in >> *std::get<BranchNode>(num.node).left >> buf >> *std::get<BranchNode>(num.node).right;
    if (buf != ',' || in.fail()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    in >> buf;
    if (buf != ']') {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::get<BranchNode>(num.node).left->position = SnailfishNumber::Position::Left;
    std::get<BranchNode>(num.node).right->position = SnailfishNumber::Position::Right;
    std::get<BranchNode>(num.node).left->parent = &num;
    std::get<BranchNode>(num.node).right->parent = &num;

    return in;
}

/* After my first attempt, I made this even more memory safe by using unique pointers. */

int main() {  
    std::vector<std::string> inputs {};

    std::string line {};
    while (std::cin >> line) {
        inputs.push_back(line);
    }

    int best_magnitude {0};
    for (int i {0}; i < static_cast<int>(inputs.size()); i++) {
        for (int j {0}; j < static_cast<int>(inputs.size()); j++) {
            if (j == i)
                continue;

            SnailfishNumber a = SnailfishNumber(); 
            std::istringstream(inputs[i]) >> a;
            SnailfishNumber b = SnailfishNumber();
            std::istringstream(inputs[j]) >> b;

            std::unique_ptr<SnailfishNumber> res = SnailfishNumber::append(std::move(a), std::move(b));
            int mag {res->magnitude()};

            if (mag > best_magnitude)
                best_magnitude = mag;
        }
    }
    
    std::cout << best_magnitude << '\n';
}
