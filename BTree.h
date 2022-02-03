#include<iostream>
#include<vector>
#include<cmath>
#include"BTreeNode.h"
using namespace std;


// Class B tree with all operations on tree
class BTree {
    private:
        BTNode* root;
        int t;

    public:
        BTree(int);
        Node* Search(int);
        Node* create_node(int, BTNode*);
        void Insert(int);
        void Delete(int);
        void Update(int);
        void traverse();

};
//////////////////////////////////////////////////////
BTree::BTree(int dgr) {
    root = NULL;
    t = dgr + 1;
}
/////////////////////////////////////////////////////
Node* BTree::Search(int k) {
    return root->search_node(k);
}
/////////////////////////////////////////////////////
Node* BTree::create_node(int data, BTNode *node) {
    Node* key = new Node;        // create element of root node
    key->data = data;               // initialize element of root node
    key->nextField = NULL;
    key->self = node;            // recognize root as array that the element is in it

    return key;
}
/////////////////////////////////////////////////////////
void BTree::Insert(int data) {
    // case 1: the tree is empty
    if (!root) {                          
        root = new BTNode(t);                // create root node
        Node* key = create_node(data, root);
        root->key[0] = key;    // add node to root node
        root->num_keys++;
    }
    // case 2: the root is not empty
    else {
        root = root->find_insert_place(data, root, t, root);
    }
 
}
///////////////////////////////////////////////////////
void BTree::Delete(int k) {
    // case 1: the tree is empty
    if (root == NULL) {
        cout << "There is nothing to delete.";
        return;
    }
    // case 2: non-empty tree
    else {
        // go deletiong from root
        root->delete_node(k, t);
    }
    
    
}
/////////////////////////////////////////////////////
void BTree::Update(int k) {
    
}
/////////////////////////////////////////////////////
void BTree::traverse() {
    root->traverse(0);
}
//////////////////////////////////////////////////////