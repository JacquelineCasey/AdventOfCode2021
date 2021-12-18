
#include <cassert>
#include <iostream>
#include <memory>
#include <variant>

class SnailfishNumber;

struct BranchNode {
public:
    SnailfishNumber* left {nullptr};
    SnailfishNumber* right {nullptr}; 
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
        current = std::get<BranchNode>(current->parent->node).left;

        /* The rightmost child of this sibling is our left neighbor */
        while (std::holds_alternative<BranchNode>(current->node)) {
            current = std::get<BranchNode>(current->node).right;
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
        current = std::get<BranchNode>(current->parent->node).right;

        /* The leftmost child of this sibling is our right neighbor */
        while (std::holds_alternative<BranchNode>(current->node)) {
            current = std::get<BranchNode>(current->node).left;
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

        delete std::get<BranchNode>(ptr->node).left;
        delete std::get<BranchNode>(ptr->node).right;

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
        new_bn.left = new SnailfishNumber();
        new_bn.right = new SnailfishNumber();

        new_bn.left->parent = ptr;
        new_bn.right->parent = ptr;
        new_bn.left->position = Position::Left;
        new_bn.right->position = Position::Right;
        new_bn.left->node = LeafNode({big_number / 2});
        new_bn.right->node = LeafNode({big_number / 2 + big_number % 2});

        ptr->node = new_bn;
        return true;
    }

public:
    friend std::istream& operator>>(std::istream& in, SnailfishNumber& num);

    static SnailfishNumber* append(SnailfishNumber* l, SnailfishNumber* r) {
        SnailfishNumber* result = new SnailfishNumber();
        BranchNode result_bn {};
        result_bn.left = l;
        result_bn.right = r;

        result_bn.left->parent = result;
        result_bn.right->parent = result;
        result_bn.left->position = SnailfishNumber::Position::Left;
        result_bn.right->position = SnailfishNumber::Position::Right;

        result->node = result_bn;

        int step {0};
        while (true) {
            step++;
            
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
    std::get<BranchNode>(num.node).left = new SnailfishNumber();
    std::get<BranchNode>(num.node).right = new SnailfishNumber();
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

/* I spent way to long debugging this, so now my code is littered with raw pointers,
 * and allocations I am too lazy to hunt down and free.
 *
 * Edit: After submission, I did go back and plug the memory leak (at least the
 * major one.) */

int main() {  
    SnailfishNumber* n = new SnailfishNumber();
    std::cin >> *n;
    
    SnailfishNumber* next = new SnailfishNumber();
    while (std::cin >> *next) {
        n = SnailfishNumber::append(n, next);
        next = new SnailfishNumber();
    }

    std::cout << n->magnitude() << '\n';

    return 0;
}
