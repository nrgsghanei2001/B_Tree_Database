#include<iostream>
#include<vector>
using namespace std;

class BTNode;
// Class Node for each elements inside elements of B-tree
class Node {
    public:
        int data;
        Node* nextField;
        BTNode* self;
};
/////////////////////////////////////////////////////
// Class for elements of B-tree
class BTNode {
    public:
        Node** key;
        BTNode** child;
        bool is_leaf;
        int num_keys;
        BTNode(int m) {
            key = new Node*[m];
            is_leaf = true;
            num_keys = 0;
            child = new BTNode*[m + 1];
            for (int i = 0; i < m; i++) {
                key[i] = NULL;
            }
            for (int i = 0; i < m + 1; i++) {
                child[i] = NULL;
            }
        }

        BTNode* find_insert_place(int, BTNode*, int, BTNode*);
        void split_nodes(BTNode*, int);
        void print_keys(BTNode*, int);
        void traverse(int);
};
/////////////////////////////////////////////////////
BTNode* BTNode::find_insert_place(int data, BTNode* curr, int t, BTNode* root) {
    // case 1: we are at leaf node
    if (is_leaf) {
        int i = t;
        // find the first null place in keys
        while (key[i - 1] == NULL) {
            i--;
        }
        // data is bigger than cuurent key, pass untill getting the right place
        while (key[i - 1]->data > data && i != 0) {
            key[i] = key[i - 1];
            i--;
        }
        // initializing the new key
        Node* new_key = new Node;
        new_key->data = data;
        new_key->nextField = NULL;
        new_key->self = curr;
        key[i] = new_key;     // put the key in the right place
    }
    // case 2: we are not at leaf
    else {
        int i = 0;
        // find the right child to insert to or get out if node is full
        while (i < num_keys && key[i]->data < data) {
            i++;
        }
        // the right child found, go recusrively to find the real place at the leaf
        child[i]->find_insert_place(data, this, t, root);   
    }
    // the current node is full
    if (num_keys == t) {
        // we are at root node
        if(this == root) {
            // create new node and split the full node
            BTNode* new_node = new BTNode(t);
            new_node->is_leaf = false;
            new_node->child[0] = this;  // the full node should be the child of new node
            new_node->split_nodes(this, t);  // split the nodes and pass one of them up to new node
            return new_node;
        }
        // we are not at root and don't need to create new node because parent of this node exists
        else {
            // just split current node
            curr->split_nodes(this, t);
        }
        
    }
    return root;
}
///////////////////////////////////////////////////////////
void BTNode::split_nodes(BTNode* curr, int t) {
    // create new node for right child
    BTNode* right = new BTNode(t);
    int r_counter = 0;
    int mid = (t - 1) / 2;  // index of median to put it up
    int number_of_keys = curr->num_keys;   // number of keys in full node
    Node* median = curr->key[mid];   // median of keys

    
    for (int i = mid + 1; i < number_of_keys; i++) {
        right->key[r_counter] = curr->key[i];        // put keys after median to right child
        r_counter++;
        curr->num_keys--;      // number of keys decreases 
        right->num_keys++;     // number of keys of right child increases
    }

    // current node has childred
    int ch_counter = 0;
    if (!curr->is_leaf) {
        for (int i = t / 2; i <= t; i++) {
            right->child[ch_counter] = curr->child[i];     // pass children of current node to new right child
            ch_counter++;
        }
        right->is_leaf = curr->is_leaf;
    }
    // do this for owning chidren and non owning chidren nodes
    // allocate place for right children
    int ri_counter = t - 1;
    // continue untill you get to current node
    while (child[ri_counter] != curr) {
        ri_counter--;
    }
    // put right child in the correct place
    child[ri_counter + 1] = right;
    // move the median up
    int mid_counter = t - 1;
    while (key[mid_counter - 1] != NULL && mid_counter != 0) {
        key[mid_counter] = key[mid_counter - 1];   // find the median location
        mid_counter--;
    }
    key[mid_counter] = median;    // put the median in it's right place in parent node
    num_keys++;
    curr->num_keys--;       // one moved up so decrease it
    
    
}
///////////////////////////////////////////////////////////
void BTNode::print_keys(BTNode* node, int level) {
    // case 1: there is nothong in our tree
    if (level > 1 && is_leaf) {
        cout << "empty!!!" << endl;
    }
    // case 2: we are in the level so print it
    else if (level == 1) {
        for (int i = 0; i < num_keys; i++) {
            cout << key[i]->data << " ";
        }
    }
    // case 3: go recursively to print all
    else {
        for (int i = 0; i < num_keys; i++) {
            child[i]->print_keys(node, level - 1);
        }
    }
}
///////////////////////////////////////////////////////////
void BTNode::traverse(int tab) {
    int i;
    string s;
 
    // Print 'tab' number of tabs
    for (int j = 0; j < tab; j++) {
        s += '\t';
    }
    for (i = 0; i < num_keys; i++) {
 
        // If this is not leaf, then before printing key[i]
        // traverse the subtree rooted with child C[i]
        if (is_leaf == false)
            child[i]->traverse(tab + 1);
        cout << s << key[i]->data << endl;
    }
 
    // Print the subtree rooted with last child
    if (is_leaf == false) {
        child[i]->traverse(tab + 1);
    }
}
/////////////////////////////////////////////////
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
        void Split(BTNode*);
        void Delete(int);
        void Update(int);
        void Traverse_level_order(int);
        void traverse();

};
//////////////////////////////////////////////////////
BTree::BTree(int dgr) {
    root = NULL;
    t = dgr;
}
/////////////////////////////////////////////////////
Node* BTree::Search(int k) {
    
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
    
    cout << "inserted"<<endl;

}
/////////////////////////////////////////////////////
void BTree::Split(BTNode *y) {
    // BTNode *z = new BTNode;
    // z->is_leaf = y->is_leaf;
    // // Copy the last (t-1) keys of y to z
    // for (int j = 0; j < 4; j++)
    //     z->key[j] = y->keys[j+t];
 
    // // Copy the last t children of y to z
    // if (!y->is_leaf) {
    //     z->child.push_back(y->child[2])
    //     for (int j = 0; j < t; j++)
    //         z->C[j] = y->C[j+t];
    // }
 
    // // Reduce the number of keys in y
    // y->n = t - 1;
 
    // // Since this node is going to have a new child,
    // // create space of new child
    // for (int j = n; j >= i+1; j--)
    //     C[j+1] = C[j];
 
    // // Link the new child to this node
    // C[i+1] = z;
 
    // // A key of y will move to this node. Find the location of
    // // new key and move all greater keys one space ahead
    // for (int j = n-1; j >= i; j--)
    //     keys[j+1] = keys[j];
 
    // // Copy the middle key of y to this node
    // keys[i] = y->keys[t-1];
 
    // // Increment count of keys in this node
    // n = n + 1;
}
///////////////////////////////////////////////////////
void BTree::Delete(int k) {
    
}
/////////////////////////////////////////////////////
void BTree::Update(int k) {
    
}
/////////////////////////////////////////////////////
void BTree::Traverse_level_order(int level) {
    root->print_keys(root, level);
}
/////////////////////////////////////////////////////
void BTree::traverse() {
    root->child[0]->traverse(0);
}
///////////////////////////////////////////////////
int main() {
    int n, t;
    cin >> n >> t;
    BTree tree(t);
    for (int i = 0; i < n; i++) {
        int w;
        cin >> w;
        tree.Insert(w);
    }
    // tree.Traverse_level_order(2);
    tree.traverse();
}