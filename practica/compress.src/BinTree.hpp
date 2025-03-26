#ifndef BINTREE_HH
#define BINTREE_HH

#include <memory>
#include <cassert>

// A BinTree<T> implements binary trees with values of type T.
template <typename T>
class BinTree {

    struct Node {
        T x;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::shared_ptr<Node> parent;

        Node (const T& x, std::shared_ptr<Node> left, std::shared_ptr<Node> right, std::shared_ptr<Node> parent)
        :   x(x), left(left), right(right), parent(parent)
        {   }

    };

    // A tree only holds a node pointer.
    std::shared_ptr<Node> p;

    // Constructs a tree from a node pointer.
    BinTree (std::shared_ptr<Node> p)
    :   p(p)
    {   }

    // Notes:
    //   - default operator=() is good.
    //   - default destructor is good. Θ(n) where n is the number of nodes in the tree.
    //   - std::swap() already works by default.

public:

    // Constructs an empty tree. Θ(1).
    BinTree ()
    :   p(nullptr)
    {   }

    // Constructs a tree with a value x and no subtrees. Θ(1).
    explicit BinTree (const T& x) {
        p = std::make_shared<Node>(x, nullptr, nullptr, nullptr);
    }

    // Constructs a tree with a value x and two subtrees left and right. Θ(1).
    explicit BinTree (const T& x, const BinTree& left, const BinTree& right) {
        p = std::make_shared<Node>(x, left.p, right.p, nullptr);
        left.p->parent = p;
        right.p->parent = p;
    }

    // Tells if this tree is empty. Θ(1).
    bool empty () const {
        return not p;
    }

    // Returns the left subtree of this tree (cannot be empty). Θ(1).
    BinTree left () const {
        assert(not empty());
        return BinTree(p->left);
    }

    // Returns the right subtree of this tree (cannot be empty). Θ(1).
    BinTree right () const {
        assert(not empty());
        return BinTree(p->right);
    }

    // Returns the parent subtree of this tree (cannot be empty). Θ(1).
    BinTree parent () const {
        assert(not empty());
        return BinTree(p->parent);
    }

    // Returns the value of this tree (cannot be empty). Θ(1).
    const T& value () const {
        assert(not empty());
        return p->x;
    }

    // Tells if two tree objects point towards the same node. Θ(1).
    bool equals (const BinTree compare){
        assert(not empty() && not compare.empty());
        return p == compare.p;
    }

    // Modifies the tree's value. Θ(1).
    void setValue (T newValue){
        p->x = newValue;
    }

    // Adds two trees as children. Θ(1).
    void addChildren (const BinTree& left, const BinTree& right){
        p->left = left.p;
        left.p->parent = p;
        p->right = right.p;
        right.p->parent = p;
    }

    
    void swap (BinTree& objective){
        std::shared_ptr<Node> aux;

        aux = p;
        p = objective.p;
        objective.p = aux;

        aux = p->parent;
        p->parent = objective.p->parent;
        objective.p->parent = aux;

        if (p->parent){
            if (p->parent->left == objective.p) p->parent->left = p;
            else p->parent->right = p;
        }
        if (objective.p->parent){
            if (objective.p->parent->left == p) objective.p->parent->left = objective.p;
            else objective.p->parent->right = objective.p;
        }
    }

};
#endif
