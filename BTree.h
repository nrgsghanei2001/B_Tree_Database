#include<iostream>
#include<vector>
#include<cmath>
#include"BTreeNode.h"
using namespace std;


// Class B tree with all operations on tree
class BTree {
    private:
        BTNode* root;
        long long int t;

    public:
        BTree(long long int);
        Node* Search(long long int);
        Node* create_node(long long int, BTNode*);
        void Insert(long long int);
        void Delete(long long int);
        void Update(long long int);
        void traverse();

};
//////////////////////////////////////////////////////
BTree::BTree(long long int dgr) {
    root = NULL;
    t = dgr + 1;
}
/////////////////////////////////////////////////////
Node* BTree::Search(long long int k) {
    return root->search_node(k);
}
/////////////////////////////////////////////////////
Node* BTree::create_node(long long int data, BTNode *node) {
    Node* key = new Node;        // create element of root node
    key->data = data;               // initialize element of root node
    key->nextField = NULL;
    key->self = node;            // recognize root as array that the element is in it

    return key;
}
/////////////////////////////////////////////////////////
void BTree::Insert(long long int data) {
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
void BTree::Delete(long long int k) {
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
void BTree::Update(long long int k) {
    
}
/////////////////////////////////////////////////////
void BTree::traverse() {
    root->traverse(0);
}
//////////////////////////////////////////////////////