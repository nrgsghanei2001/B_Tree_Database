#include "BTreeNode.h"
#include <vector>
#include <iostream>
using namespace std;


// Class B tree with all operations on tree
class BTree {
    private:
        BTNode* root;                    // root of the tree
        int t;                           // maximum number of keys in a tree node

    public:
        BTree(int);                                                                          // constructor   
        BTNode* get_root();                                                                  // get the tree's root     
        Node*   create_node(long long int, BTNode*);                                         // create new Node object
        Node*   Insert(long long int);                                                       // insert data to keys
        Node*   Search(long long int);                                                       // search for the given data
        void    Delete(Node*);                                                               // delete given node from tree
        void    check_for_equal(BTNode*, long long int, vector<Node*> &);                    // find all equal nodes to given node data
        void    check_for_greater(BTNode*, long long int, vector<Node*> &);                  // find all nodes that are greater then the given node data
        void    check_for_smaller(BTNode*, long long int, vector<Node*> &);                  // find all nodes that are smaller then the given node data
        void    traverse();                                                                  // print the tree
        // void Update(long long int);
        // Node* search_for_greaters(long long int);
        // Node* search_for_smaller(long long int k);
};
///////////////////////////////////////////////////////////////////////////////////////////////////
BTree::BTree(int dgr) {
    root = NULL;                                        // set the root null
    t    = dgr + 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTree::create_node(long long int data, BTNode *node) {
    Node* key      = new Node;           // create element of root node
    key->data      = data;               // initialize element of root node
    key->nextField = NULL;               // it does not point to any node at first
    key->self      = node;               // recognize node as array that the element is in it

    return key;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
BTNode* BTree::get_root() {
    return root;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTree::check_for_equal(BTNode* node, long long int h, vector<Node*> & result) {
    int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then go to the subtree of child
        if (node->is_leaf == false)
            check_for_equal(node->child[i], h, result);
        
        // if the node's data is equal to h then push it to results array
        if (node->key[i]->data == h) {
            result.push_back(node->key[i]);
        }
    }
 
    // subtree of last child
    if (node->is_leaf == false) {
        check_for_equal(node->child[i], h, result);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTree::check_for_greater(BTNode* node, long long int h, vector<Node*> & result) {
    int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then go to the subtree of child
        if (node->is_leaf == false)
            check_for_greater(node->child[i], h, result);

        // if the node's data is greater than h then push it to results array    
        if (node->key[i]->data > h) {
            result.push_back(node->key[i]);
        }
    }
 
    // subtree of last child
    if (node->is_leaf == false) {
        check_for_greater(node->child[i], h, result);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTree::check_for_smaller(BTNode* node, long long int h, vector<Node*> & result) {
    int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then go to the subtree of child
        if (node->is_leaf == false)
            check_for_smaller(node->child[i], h, result);

        // if the node's data is greater than h then push it to results array
        if (node->key[i]->data < h) {
            result.push_back(node->key[i]);
        }
    }
 
    // subtree of last child
    if (node->is_leaf == false) {
        check_for_smaller(node->child[i], h, result);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTree::Insert(long long int data) {
    Node* new_key = new Node;
    // CASE 1: the tree is empty
    if (!root) {                          
        root         = new BTNode(t);                           // create root node
        Node* key    = create_node(data, root);
        root->key[0] = key;                                     // add node to root node
        root->num_keys++;                                       // add to number of root's key
        return root->key[0];                                    // return the inserted node
    }
    // CASE 2: the root is not empty
    else {
        root = root->insertion(data, root, root, new_key);
        return new_key;                                         // return the inserted node
    }   
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTree::Delete(Node* k) {
    // CASE 1: the tree is empty
    if (root == NULL) {
        return;
    }
    // CASE 2: non-empty tree
    else {
        // go deletiong from root
        root->deletion(k);
    }  
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTree::Search(long long int k) {
    return root->search_node(k);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTree::traverse() {
    root->traverse(0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
