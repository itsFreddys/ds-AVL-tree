#ifndef __PROJ_FOUR_AVL_HPP
#define __PROJ_FOUR_AVL_HPP

#include "runtimeexcept.hpp"
#include <string>
#include <vector>
#include <iostream>

class ElementNotFoundException : public RuntimeException
{
public:
    ElementNotFoundException(const std::string & err) : RuntimeException(err) {}
};


template<typename Key, typename Value>
class MyAVLTree
{
private:
    class Node {
    public:
        Key key;
        Value val;
        Node* right;
        Node* left;
        unsigned height;

        Node(Key k, Value v): key(k),val(v),
                              right(nullptr), left(nullptr), height(0) {};

    };
    size_t tree_size;

public:
    MyAVLTree();

    Node* root;

    // The destructor is, however, required.
    ~MyAVLTree();

    // size() returns the number of distinct keys in the tree.
    size_t size() const noexcept;

    // isEmpty() returns true if and only if the tree has no values in it.
    bool isEmpty() const noexcept;

    // contains() returns true if and only if there
    //  is a (key, value) pair in the tree
    //	that has the given key as its key.
    bool contains(const Key & k) const noexcept;

    // find returns the value associated with the given key
    // If !contains(k), this will throw an ElementNotFoundException
    // There needs to be a version for const and non-const MyAVLTrees.
    Value & find(const Key & k);
    const Value & find(const Key & k) const;

    // Inserts the given key-value pair into
    // the tree and performs the AVL re-balance
    // operation, as described in lecture.
    // If the key already exists in the tree,
    // you may do as you please (no test cases in
    // the grading script will deal with this situation)
    void insert(const Key & k, const Value & v);

    // Returns the height of given key in the tree
    unsigned height(const Key& k) const;

    // in general, a "remove" function would be here
    // It's a little trickier with an AVL tree
    // and I am not requiring it for this quarter's ICS 46.
    // You should still read about the remove operation
    // in your textbook.

    // The following three functions all return
    // the set of keys in the tree as a standard vector.
    // Each returns them in a different order.
    std::vector<Key> inOrder() const;
    std::vector<Key> preOrder() const;
    std::vector<Key> postOrder() const;

    const bool search(const Key& k, Node *& n) const;
    Node* recurInsert(Node*& parent, const Key& k, const Value& v);
    void recurInOrder(const Node* parent, std::vector<Key>& v) const;
    void recurPre(const Node* parent, std::vector<Key>& v) const;
    void recurPost(const Node* parent, std::vector<Key>& v) const;

    const int get_height(Node* n);
    Node* rightRot(Node* n);
    Node* leftRot(Node* n);

    void deltree(Node* root);

};


template<typename Key, typename Value>
MyAVLTree<Key,Value>::MyAVLTree()
{
    root = nullptr;
    tree_size = 0;
}

template<typename Key, typename Value>
MyAVLTree<Key,Value>::~MyAVLTree()
{   //all handled inside recurr helper func.
    // traverse first starting from the left
    // traverse starting from the right
    // finally, delete the root node

    deltree(root);
    delete root;
}

template<typename Key, typename Value>
size_t MyAVLTree<Key, Value>::size() const noexcept
{
    return tree_size;
}

template<typename Key, typename Value>
bool MyAVLTree<Key, Value>::isEmpty() const noexcept
{
    if (tree_size == 0){
        return true;
    }
    return false;
}

template<typename Key, typename Value>
bool MyAVLTree<Key, Value>::contains(const Key &k) const noexcept
{
    Node* temp;
    if (search(k, temp)){
        return true;
    }
    return false;

}

template<typename Key, typename Value>
Value & MyAVLTree<Key, Value>::find(const Key & k)
{
    Node* temp;
    if (search(k, temp)){
        return temp->val;
    }
    throw ElementNotFoundException("key not found.");
}

template<typename Key, typename Value>
const Value & MyAVLTree<Key, Value>::find(const Key & k) const
{
    Node* temp;
    if (search(k, temp)){
        return temp->val;
    }
    throw ElementNotFoundException("key not found.");
}

template<typename Key, typename Value>
void MyAVLTree<Key, Value>::insert(const Key & k, const Value & v)
{
//    Node* temp;
    if (isEmpty()) { root = new Node(k,v); tree_size++; root->height=0; return;}
    else{root = recurInsert(root, k, v);}
    tree_size++;

}

template<typename Key, typename Value>
unsigned MyAVLTree<Key, Value>::height(const Key & k) const
{
    Node* n;
    if (search(k,n)){
        return n->height;
    }
    // std::cout<<"no key found, no height"<<std::endl;
    throw ElementNotFoundException("no key found");
}

template<typename Key, typename Value>
std::vector<Key> MyAVLTree<Key, Value>::inOrder() const
{
    std::vector<Key> v;
    recurInOrder(root, v);
    return v;
}

template<typename Key, typename Value>
std::vector<Key> MyAVLTree<Key, Value>::preOrder() const
{
    std::vector<Key> v;
    recurPre(root, v);
    return v;
}

template<typename Key, typename Value>
std::vector<Key> MyAVLTree<Key, Value>::postOrder() const
{
    std::vector<Key> v;
    recurPost(root, v);
    return v;
}

template<typename Key, typename Value>
const bool MyAVLTree<Key, Value>::search(const Key& k, Node *& n) const
{
    if (isEmpty()){ return false;}

    Node* temp = root;
    while (temp != nullptr){
        if (temp->key == k){ n = temp; return true;}
        if (temp->key > k){ temp = temp->left;}
        else if (temp->key < k) { temp = temp->right;}
    }

    return false;
}

template<typename Key, typename Value>
typename MyAVLTree<Key, Value>::Node* MyAVLTree<Key, Value>::recurInsert(Node* &parent, const Key& k, const Value& v)
{
    // basic bst insert
    if (parent == nullptr)
    {
        parent = new Node(k, v);
        return parent;
    }

    if (k < parent->key)
    {
        parent->left = recurInsert(parent->left, k, v);
    }
    else if (k > parent->key)
    {
        parent->right = recurInsert(parent->right, k, v);
    }

    // give heights to nodes, compare left side to right side and get max + 1
    parent->height = 1 + std::max(get_height(parent->left), get_height(parent->right));

    // Check the balance
    int balance = (get_height(parent->left) - get_height(parent->right));

    // std::cout<<"balance: "<<balance<<" Node: "<<parent->key<<std::endl;

    // right rotation
    if (balance > 1 && k < parent->left->key)
    {
        return rightRot(parent);
    }
    // left right rotation
    //
    else if (balance > 1 && k > parent->left->key)
    {
        parent->left = leftRot(parent->left);
        return rightRot(parent);
    }
    // left rotation
    else if (balance < -1 && k > parent->right->key)
    {
        return leftRot(parent);
    }
    // right left rotation
    //
    else if (balance < -1 && k < parent->right->key)
    {
        parent->right = rightRot(parent->right);
        return leftRot(parent);
    }

    return parent;
}


template<typename Key, typename Value>
void MyAVLTree<Key, Value>::recurInOrder(const Node* parent, std::vector<Key>& v) const
{
    if (parent == nullptr) return;

    recurInOrder(parent->left, v);
    v.push_back(parent->key);
    // std::cout<<"key: "<<parent->key<<" height: "<<parent->height<<std::endl;
    recurInOrder(parent->right, v);
}

template<typename Key, typename Value>
void MyAVLTree<Key, Value>::recurPre(const Node* parent, std::vector<Key>& v) const
{
    if (parent == nullptr) return;
    v.push_back(parent->key);
    // std::cout<<"key: "<<parent->key<<" height: "<<parent->height<<std::endl;
    recurPre(parent->left, v);
    recurPre(parent->right, v);
}

template<typename Key, typename Value>
void MyAVLTree<Key, Value>::recurPost(const Node* parent, std::vector<Key>& v) const
{
    if (parent == nullptr) return;

    recurPost(parent->left, v);
    recurPost(parent->right, v);
    v.push_back(parent->key);
    // std::cout<<"key: "<<parent->key<<" height: "<<parent->height<<std::endl;

}

template<typename Key, typename Value>
void MyAVLTree<Key, Value>::deltree(Node* n)
{
    if (n == nullptr) return;

    deltree(n->left);
    deltree(n->right);
    if (n != root){delete n;}
}

template<typename Key, typename Value>
const int MyAVLTree<Key, Value>::get_height(Node* n)
{
    if (n == nullptr){ return -1;}
    return n->height;
}

template<typename Key, typename Value>
typename MyAVLTree<Key, Value>::Node* MyAVLTree<Key, Value>::rightRot(Node* n)
{
    Node* y = n->left;
    Node* child = y->right;

    y->right = n;
    n->left = child;

    // Update heights
    n->height = 1 + std::max(get_height(n->left), get_height(n->right));
    y->height = 1 + std::max(get_height(y->left), get_height(y->right));

    return y;
}

template<typename Key, typename Value>
typename MyAVLTree<Key, Value>::Node* MyAVLTree<Key, Value>::leftRot(Node* n)
{
    Node* y = n->right;
    Node* child = y->left;

    y->left = n;
    n->right = child;

    // Update heights
    n->height = 1 + std::max(get_height(n->left), get_height(n->right));
    y->height = 1 + std::max(get_height(y->left), get_height(y->right));

    return y;
}


#endif