#include <iostream>
#include <cmath>
#include <vector>
using namespace std;


using namespace std;


class BTNode;

// Class Node for each elements inside elements of B-tree
class Node {
    public:
        long long int data;          
        Node*         nextField;           
        BTNode*       self;                              // point to cuurent btree node that this node is inside of it
};
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class for elements of B-tree
class BTNode {
    public:
        Node**        key;        
        BTNode**      child;                
        bool          is_leaf;                           // true if the node is in leaf
        long long int num_keys;                          // number of existance keys in bTree Node
        int           max_child, max_key, min_key;    

        BTNode(int);                                                     // constructor
        Node*   predecessor(int);                                        // get the maximum key before given key
        Node*   successor(int);                                          // get the minimum key before given key
        Node*   search_node(long long int);                              // search for a data in tree node
        BTNode* insertion(long long int, BTNode*, BTNode*, Node* &);     // insert a new Node to keys of BTNode
        void    deletion(Node*);                                         // delete given key from BTNode
        void    trim(BTNode*);                                           // cut a node to 2 nodes when it has more than allowed key
        void    traverse(int);                                           // print the tree

};
///////////////////////////////////////////////////////////////////////////////////////////////////
BTNode::BTNode(int dgr) {
    max_key   = dgr;
    min_key   = dgr / 2;
    max_child = dgr + 1;
    
    key       = new Node*[max_key];         // array of keys
    child     = new BTNode*[max_child];     // array of children
    is_leaf   = true;                       // every new node is a leaf
    num_keys  = 0;                          // number of keys of a new node is 0 at first
    
    // first value of each key
    for (int i = 0; i < max_key; i++) {
        key[i] = NULL;
    }
    // first value of each child
    for (int i = 0; i < max_child; i++) {
        child[i] = NULL;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
BTNode* BTNode::insertion(long long int data, BTNode* curr, BTNode* root, Node* & new_key) {
    // CASE 1: we are at leaf node
    if (is_leaf) {
        int i = max_key;
        // find the first null place in key
        while (key[i - 1] == NULL) {
            i--;
        }
        // data is bigger than cuurent key, get one step backward untill getting the right place
        while (i != 0 && key[i - 1]->data > data) {
            key[i] = key[i - 1];
            i--;
        }
        // initializing the new key
        new_key->data      = data;
        new_key->nextField = NULL;
        new_key->self      = curr;
        key[i]             = new_key;       // put the key in the right place
        num_keys++;                         // increase of keys
    }
    // CASE 2: we are not at leaf
    else {
        int i = 0;
        // find the right child to insert to or get out if node is full
        while (i < num_keys && key[i]->data < data) {
            i++;
        }
        // the right child found, go recusrively to find the real place at the leaf
        child[i]->insertion(data, this, root, new_key);   
    }
    // the current node is full
    if (num_keys == max_key) {
        // we are at root node
        if (this == root) {
            // create new node and split the full node
            BTNode* new_node   = new BTNode(max_key);
            new_node->is_leaf  = false;
            new_node->child[0] = this;  // the full node should be the child of new node
            new_node->trim(this);       // split the nodes and pass one of them up to new node
            return new_node;
        }
        // we are not at root and don't need to create new node because parent of this node exists
        else {
            // just split current node
            curr->trim(this);
        }   
    }
    return root;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTNode::deletion(Node* k) {
    // find the parent node place or exact place of accurance
    int key_counter = 0;
    // do untill getting the key that is greater than k
    while (key_counter < num_keys && key[key_counter]->data < k->data) {
        key_counter++;
    }
    // key_counter is the next place we should go
    if (key_counter < num_keys && key[key_counter]->data == k->data && key[key_counter] != k) {
        key_counter++;
    } 
    // CASE 1: k is exactly in this node
    if (key_counter < num_keys && key[key_counter] == k) {
        // CASE 1-1: delete from leaf
        if (is_leaf) {
            // just move next key one step backward
            for (long long int i = key_counter + 1; i < num_keys; i++) {
                key[i - 1] = key[i];
            }
            // reduce the number of key by 1
            key[num_keys - 1] = NULL;
            // delete(key[num_keys - 1]);
            num_keys--;
        }
        // CASE 1-2: delete from non-leaf node
        else {
            Node* k2 = key[key_counter];
            // CASE 1-2-1: this key's child has at least t/2 key
            if (child[key_counter]->num_keys >= min_key) {
                Node* pre = predecessor(key_counter);
                // replace the predecessor with deleted key
                key[key_counter] = pre;
                // recursively remove the predesessor
                child[key_counter]->deletion(pre);
            } 
            // CASE 1-2-2: this key's child has not enough children so test his right brother
            else if (child[key_counter + 1]->num_keys >= min_key) {
                Node* suc        = successor(key_counter);
                // replace the successor with deleted key
                key[key_counter] = suc;
                // recursively remove the successor
                child[key_counter + 1]->deletion(suc);
            }
            // CASE 1-2-3: non of them have enough children so merge 2 children
            else {
                BTNode* lchild           = child[key_counter];
                BTNode* rchild           = child[key_counter + 1];
                // put the key in rchild
                lchild->key[min_key - 1] = key[key_counter];
                // move right child key and children to left child 
                for (int i = 0; i < rchild->num_keys; i++) {
                    lchild->key[i + min_key] = rchild->key[i];
                    // if it has any children pass them to left child
                    if (lchild->is_leaf == false) {
                        lchild->child[i + min_key] = rchild->child[i];
                    }
                    if (lchild->is_leaf == false && i == rchild->num_keys - 1) {
                        lchild->child[i + 1 + min_key] = rchild->child[i + 1];
                    }
                }
                // move key 1 step and children 2 step backward
                for (int i = key_counter + 1; i < num_keys; i++) {
                    key[i - 1] = key[i];
                    child[i]   = child[i + 1];
                }

                // number of key in left child increased 
                lchild->num_keys += rchild->num_keys + 1;
                // number of key in this node decreased
                key[num_keys - 1] = NULL;
                num_keys--;
                // free the memory of right child after merging
                delete(rchild);
                // recursively remove the key that is in child now
                child[key_counter]->deletion(k2);
            }
        }
    }
    // CASE 2: k is not accured in this node
    else {
        // CASE 2-1: we are at leaf node and there is not any chilren so the key was not found
        if (is_leaf) {
            return;
        }
        // CASE 2-2: the key is in subtrees of this node
        else {
            // CASE 2-2-1: key is in last key child and the child has more than allowed key
            // check if it is the last child
            int last_child = 0;
            if (key_counter == num_keys) {
                last_child = 1;
            }
            if (child[key_counter]->num_keys < min_key) {
                // CASE 2-2-1-1: borrow key from left brother
                if (key_counter != 0 && child[key_counter - 1]->num_keys >= min_key) {
                    BTNode* rrchild = child[key_counter];
                    BTNode* llchild = child[key_counter - 1];
                    
                    // move children and children of it forward
                    for (int i = rrchild->num_keys - 1; i >= 0; i--) {
                        rrchild->key[i + 1] = rrchild->key[i];
                        // it has children
                        if (rrchild->is_leaf == false) {
                            rrchild->child[i + 1] = rrchild->child[i];
                        }
                        if (rrchild->is_leaf == false && i == rrchild->num_keys - 1) {
                            rrchild->child[i + 2] = rrchild->child[i + 1];
                        }
                    }
                    // pass the children
                    rrchild->key[0] = key[key_counter - 1];
                    if (rrchild->is_leaf == false) {
                        rrchild->child[0] = llchild->child[llchild->num_keys];
                    }
                    // move the right child's key up
                    key[key_counter - 1] = llchild->key[llchild->num_keys - 1];
                    // update number of key
                    rrchild->num_keys++;
                    llchild->num_keys--;
                }
                // CASE 2-2-1-2: borrow key from right brother
                else if (key_counter != num_keys && child[key_counter + 1]->num_keys >= min_key) {
                    BTNode* llchild = child[key_counter];
                    BTNode* rrchild = child[key_counter + 1];

                    // move key as a child
                    llchild->key[llchild->num_keys] = key[key_counter];
                    
                    // move children of children 
                    if (llchild->is_leaf == false) {
                        llchild->child[llchild->num_keys + 1] = rrchild->child[0];
                    }
                    // pass the key
                    key[key_counter] = rrchild->key[0];
                    // move key and children of right child backward
                    for (int i = 1; i < rrchild->num_keys; i++) {
                        rrchild->key[i - 1] = rrchild->key[i];
                    }
                    // if it has any children
                    if (rrchild->is_leaf == false) {
                        for (long long int i = 0; i < rrchild->num_keys; i++) {
                            rrchild->child[i] = rrchild->child[i + 1];
                        }
                    }
                    
                    // update number of key
                    rrchild->num_keys--;
                    llchild->num_keys++;
                }
                // CASE 2-2-1-3: we can't borrow from brothers so merge with next child
                else if (key_counter != num_keys) {
                    BTNode* lchild           = child[key_counter];
                    BTNode* rchild           = child[key_counter + 1];
                    // put the key in rchild
                    lchild->key[min_key - 1] = key[key_counter];
                    // move right child key and children to left child 
                    for (int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + min_key] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + min_key] = rchild->child[i];
                        }
                        if (lchild->is_leaf == false && i == lchild->num_keys - 1) {
                            lchild->child[i + 1 + min_key] = rchild->child[i + 1];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (int i = key_counter + 1; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i]   = child[i + 1];
                    }

                    // number of key in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of key in this node decreased
                    key[num_keys - 1] = NULL;
                    num_keys--;
                    // free the memory of right child after merging
                    delete(rchild);
                }
                // CASE 2-2-1-4: we can't borrow from brothers so merge with last child
                else {
                    BTNode* lchild           = child[key_counter - 1];
                    BTNode* rchild           = child[key_counter];
                    // put the key in rchild
                    lchild->key[min_key - 1] = key[key_counter - 1];
                    // move right child key and children to left child 
                    for (int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + min_key] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + min_key] = rchild->child[i];
                        }
                        if (lchild->is_leaf == false && i == lchild->num_keys - 1) {
                            lchild->child[i + 1 + min_key] = rchild->child[i + 1];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (int i = key_counter; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i]   = child[i + 1];
                    }

                    // number of key in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of key in this node decreased
                    key[num_keys - 1] = NULL;
                    num_keys--;
                    // free the memory of right child after merging
                    delete(rchild);
                }
            }
            // CASE 2-2-2: children are merged
            if (last_child && key_counter > num_keys) {
                // recursively delete
                child[key_counter - 1]->deletion(k);
            }
            // CASE 2-2-3: children are not merged or key_counter does not increased
            else {
                child[key_counter]->deletion(k);
            }
        }
    }
    return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTNode::trim(BTNode* curr) {
    // create new node for right child
    BTNode* right           = new BTNode(max_key);
    int r_counter = 0;
    int mid                 = (max_key - 1) / 2;            // index of median to put it up
    int number_of_keys      = curr->num_keys;               // number of key in full node
    Node* median            = curr->key[mid];               // median of key
    
    for (int i = mid + 1; i < number_of_keys; i++) {
        right->key[r_counter] = curr->key[i];        // put key after median to right child
        r_counter++;
        curr->num_keys--;                            // number of key decreases 
        right->num_keys++;                           // number of key of right child increases
    }
    // current node has childred
    int ch_counter = 0;
    if (!curr->is_leaf) {
        for (int i = ceil(float(max_key) / 2); i <= max_key; i++) {
            right->child[ch_counter] = curr->child[i];     // pass children of current node to new right child
            ch_counter++;
        }
        right->is_leaf = curr->is_leaf;
    }
    // do this for owning chidren and non owning chidren nodes
    // allocate place for right children
    int ri_counter = max_key - 1;
    // continue untill you get to current node
    while (child[ri_counter] != curr) {
        child[ri_counter+1] = child[ri_counter];
        ri_counter--;
    }
    // put right child in the correct place
    child[ri_counter + 1] = right;
    // move the median up
    int mid_counter = max_key - 1;
    while (mid_counter != 0 && !key[mid_counter - 1]) {
        // move untill getting the median location
        mid_counter--;
    }

    while(mid_counter != 0 && key[mid_counter - 1]->data > median->data) {
        // change and replace untill getting the median location
        key[mid_counter] = key[mid_counter - 1];
        mid_counter--;
    }

    key[mid_counter] = median;    // put the median in it's right place in parent node
    num_keys++;
    curr->num_keys--;             // one moved up so decrease it
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTNode::predecessor(int key_counter) {
    BTNode* node = child[key_counter];
    // get the right most node
    while (node->is_leaf == false) {
        node = node->child[node->num_keys];
    }

    return node->key[node->num_keys-1]; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTNode::successor(int key_counter) {
    BTNode* node = child[key_counter + 1];
    // get the left most node
    while (node->is_leaf == false) {
        node = node->child[0];
    }

    return node->key[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void BTNode::traverse(int tab) {
    int i;
    string s;
 
    // Print 'tab' number of tabs
    for (long long int j = 0; j < tab; j++) {
        s += '\t';
    }
    for (i = 0; i < num_keys; i++) {
 
        // If this is not leaf, then before printing key[i] traverse the subtree rooted with child C[i]
        if (is_leaf == false)
            child[i]->traverse(tab + 1);
        cout << s << key[i]->data << endl;
    }
 
    // Print the subtree rooted with last child
    if (is_leaf == false) {
        child[i]->traverse(tab + 1);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* BTNode::search_node(long long int k) {
    int key_counter = 0;
    // do untill you get the place of accurance of node in it or in it's subtrees
    while (key_counter < num_keys && key[key_counter]->data < k) {
        key_counter++;
    }
    // CASE 1: the node contains key
    if (key_counter < num_keys && key[key_counter]->data == k) {
        return key[key_counter];
    }
    // CASE 2: the key is not in this node and node does not have any children, so its not found
    else if (is_leaf) {
        return NULL;
    }
    // CASE 3: it may be in subtrees, so search recursively for it
    else {
        return child[key_counter]->search_node(k);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
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

// This function hashes strings input containing small letters and numbers
long long int hash_string(string s) {
    // hash table for saving code of each character
    char hash_table[37] = {'/','0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    
    long long int hashed = 0;
    for (int i = 0; i < s.length(); i++) {
        char ch = s[i];
        int  h  = 0;
        for (int j = 0; j < 37; j++) {
            if (ch == hash_table[j]) {
                h = j;                      // find character in table
                break;
            }
        }
        hashed += h * floor(pow(37, i));   // add to result
    }
    return hashed;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// Convert the hashed number to string
string dehash_string(long long int hashed) {
    // hash table for saving code of each character
    char hash_table[37] = {'/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    string dehash = "";
    // turn to base 37 and find the answer
    while (hashed != 0) {
        int d   = hashed % 37;
        dehash += hash_table[d];
        hashed /= 37;
    }
    return dehash;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function hashes date input
long long int hash_date(string date) {
    long long int h = 0;
    int first       = 0,
    second          = 0;            
    // keeps the index of first and second / in date
    for (int i = 0; i < date.length(); i++) {
        if (date[i] == '/' && !first) {
            first = i;
        }
        else if (date[i] == '/' && !second) {
            second = i;
        }
    }
    string year  = date.substr(0, 4);
    string month = date.substr(first + 1, second - first - 1);
    string day   = date.substr(second + 1);
    // convert the number od year, month and day to int
    h += stoi(year);
    h *= 100;
    h += stoi(month);
    h *= 100;
    h += stoi(day);

    return h;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// Convert hashed number to date object
string dehash_date(long long int d) {
    string dehash = "";
    string date   = to_string(d);
    string year   = date.substr(0, 4);     // 4 first number is year
    string month  = date.substr(4, 2);     // 2 second number is month
    string day    = date.substr(6, 2);     // 2 third number is day
    dehash        += year;
    dehash        += '/';

    if (month[0] == '0') {                  // if month is single digit ignore the ziro
        dehash += month[1];
    }
    else {
        dehash += month;
    }
    dehash += '/';
    if (day[0] == '0') {                   // if day is single digit ignore the ziro
        dehash += day[1];
    }
    else {
        dehash += day;
    }

    return dehash;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input string to be valid (small letters and numbers)
bool check_string_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        // not small letter
        if (int(s[i]) < 97 || int(s[i]) > 122) {
            // not number
            if (!(int(s[i]) >= 48 && int(s[i]) <= 57)) {
                return false;
            }
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input date to be valid
bool check_date_validation(string d) {
    if (d.length() > 10 || d.length() < 8) {
        return false;             // not enough or more than enough characters
    }
    int slash = 0, f_slash, s_slash;
    // find the index of /'s in date
    for (int i = 0; i < d.length(); i++) {
        if (d[i] == '/') {
            if (slash == 0) {
                f_slash = i;
            }
            else {
                s_slash = i;
            }
            slash++;
        }
    }
    if (slash != 2 || f_slash != 4) {
        return false;       // date should contaion 2 /'s and first slash should be in 4'th place
    }
    if (!(s_slash == 6 || s_slash == 7)) {
        return false;       // second slash should be in 6 or 7'th place
    }
    // invalid characters
    for (int i = 0; i < d.length(); i++) {
        if (int(d[i]) > 57 || int(d[i]) < 47) {
            return false;
        }
    }
    // find the month and check it be between 1 and 12
    string month = d.substr(5, s_slash - f_slash);
    int    mon   = stoi(month);
    if (mon < 1 || mon > 12) {
        return false;
    }
    // find the day and check it be between 1 and 31
    string day = d.substr(s_slash + 1);
    int    dd  = stoi(day);
    if (dd < 1 || dd > 31) {
        return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input int to be valid
bool check_int_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        // invalid charcters (not between 0 to 9)
        if (int(s[i]) < 48 || int(s[i]) > 57) {
            return false;
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
vector<string> parse_query(string input) {
    string inputed = "";
    // delete "" from strings
    for (int i = 0; i < input.length(); i++) {
        if (int(input[i]) != 34) {
            inputed += input[i];
        }
    }

    input       = inputed;
    string word = "";
    vector<string> tokens;                         // save all of tokens 
    // move on query line
    for (int i = 0; i < input.length(); i++) {
        // when got to space just ignore it and add created word till now as a token
        if (input[i] == ' ') {
            if (word != " " && word != "") {
                tokens.push_back(word);
            }
            word = "";
        }
        // when got to ( ignore ( , ) and space
        else if (input[i] == '(') {
            for (int j = i + 1; input[j] != ')'; i++, j++) {
                if (input[j] == ' ' || input[j] == ',') {
                    if (word != " " && word != "") {
                        tokens.push_back(word);
                    }
                    word = "";
                }
                else {
                    word += input[j];
                }
            }
        }
        // add to token word
        else {
            if (input[i] != ')') {
                word += input[i];
            }
            // for the last token of line
            if (i == input.length() - 1 && word != " " && word != "") {
                tokens.push_back(word);
            }
        }
    }
    return tokens;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
class Table {
    private:
        int                    table_id;                     // tables unique id
        bool*                  id_table;                     // array of all ids of entries on table
        string                 table_name;                   // name of table object
        vector<vector<string>> fields;                       // keeps the name and type of fields of table
        vector<BTree*>         columns;                      // keeps Btrees of columns
    
    public:
        Table(string);                                                                            // constructor
        string                 get_name();                                                        // get the name of table
        vector<vector<string>> select(vector<string>, int);                                       // get all records that contain condition
        long long int          find_id_to_insert();                                               // get the first free id
        int                    get_id();                                                          // returns table's id
        void                   create_filed(string, string);                                      // add new field to table
        void                   set_id(int);                                                       // set a unique id to table
        void                   insertion(vector<string>);                                         // insert a record to table
        void                   insert_for_update(vector<string>, vector<long long int>, bool);    // insert record of update mode
        void                   deletion(vector<string>);                                          // delete a record from table
        void                   deletion_for_update(vector<string>);                               // delete datas which is going to update
        void                   deletion_for_update_or(vector<string>);                            // delete datas which is going to update for |
        void                   deletion_for_update_and(vector<string>);                           // delete datas which is going to update for &
        void                   deletion_or(vector<string>);                                       // delete for 2 conditions seprated with |
        void                   deletion_and(vector<string>);                                      // delete for 2 conditions seprated with &
        void                   update(vector<string>);                                            // update a record of table
        void                   update_or(vector<string>);                                         // update a record of table 2 condition with |
        void                   update_and(vector<string>);                                        // update a record of table 2 condition with &
        void                   show_fields();                                                     // show fields of table
        void                   show_columns_info();                                               // show all fields with their entries

};
///////////////////////////////////////////////////////////////////////////////////////////////////
Table::Table(string name) {
    table_name = name;                // initializing the tables name
    fields.push_back({});             // fields[0] = name of fields
    fields.push_back({});             // fileds[1] = type of fields

    // create am array for ids to find the first non-used id
    id_table = new bool[1000000];
    for (long long int i = 0; i < 1000000; i++) {
        id_table[i] = true;           // true = non-used
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
string Table::get_name() {
    return table_name;
}
///////////////////////////////////////////////////////
void Table::create_filed(string field_name, string field_type) {
    // supported types: string, date, int
    if (field_type == "string" || field_type == "timestamp" || field_type == "int") {
        fields[0].push_back(field_name); 
        fields[1].push_back(field_type);
        columns.push_back(new BTree(5));       // create the column's tree
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::set_id(int id) {
    table_id = id;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int Table::get_id() {
    return table_id;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::show_fields() {
    for (int i = 0; i < fields[0].size(); i++) {
        cout << i + 1 << "-" << fields[0][i] << " " << fields[1][i] << endl;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::show_columns_info() {
    for (int i = 0; i < columns.size(); i++) {
        cout << fields[0][i] << endl;
        columns[i]->traverse();
        cout << endl<<"----------------"<<endl;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
long long int Table::find_id_to_insert() {
    long long int id;
    // check all the array of ids to find the first free one
    for (long long int i = 1; i < 1000000; i++) {
        if (id_table[i]) {
            id = i;
            break;
        }
    }
    return id;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::insertion(vector<string> tokens) {
    int num_fields = tokens.size() - 4;                // find number of records in tokens list
    // number of fileds is not correct
    if (num_fields != columns.size() - 1) {
        return;
    }
    else {
        vector<Node*> inserted_list;                   // each inserted node kept in this array
        long long int id = find_id_to_insert();        // find the first free id
        Node* insert     = columns[0]->Insert(id);     // node of id
        id_table[id]     = false;                      // mark this id as used
        inserted_list.push_back(insert);       

        long long int h;
        // hash each record
        for (int i = 4; i < tokens.size(); i++) {
            string type_of = fields[1][i - 3];         // find the type if related column
            // convert to right type and check the type's validation
            if (type_of == "string") {
                try {
                    if (check_string_validation(tokens[i])) {     
                    h = hash_string(tokens[i]);
                    }
                    else {
                        throw "ERROR: string should contain only small letters and numbers";
                    }
                }
                catch(char const* e) {
                    cerr << e << endl;                 // show the error if accured
                }
            }
            else if (type_of == "timestamp") {
                try {
                    if (check_date_validation(tokens[i])) {
                        h = hash_date(tokens[i]);
                    }
                    else {
                        throw "ERROR: Invalid type of date";
                    }
                }
                catch(char const* e) {
                    cerr << e << endl;                 // show the error if accured
                }
            }
            else {
                try {
                    if (check_int_validation(tokens[i])) {
                        h = stoll(tokens[i]);
                    }
                    else {
                        throw "ERROR: Invalid type of number";
                    }
                }
                catch(char const* e) {
                    cerr << e << endl;                 // show the error if accured
                }
            }
            insert = columns[i - 3]->Insert(h);        // insert to right column if everything goes well
            inserted_list.push_back(insert);
        }

        // fix next_fields of inserted nodes
        for (int i = 0; i < inserted_list.size() - 1; i++) {
            inserted_list[i]->nextField = inserted_list[i + 1];
        }
        // for the last field
        inserted_list[inserted_list.size() - 1]->nextField = inserted_list[0];
    } 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::insert_for_update(vector<string> tokens, vector<long long int> ids, bool sinle_condition) {
    int num_fields;
    if (sinle_condition)
        num_fields = tokens.size() - 5;                                // find number of records in tokens list
    else
        num_fields = tokens.size() - 7;
    // number of fileds is not correct
    if (num_fields != columns.size() - 1) {
        return;
    }
    else {
        // insert for all deleted ids
        for (int k = 0; k < ids.size(); k++) {
            vector<Node*> inserted_list;                                  // keep all inserted datas in it
            long long int id = ids[k];           
            Node* insert     = columns[0]->Insert(id);                    // insert the deleted id again
            inserted_list.push_back(insert);

            long long int h;
            // hash each record
            for (int i = 3; i < columns.size() + 2; i++) {
                string type_of = fields[1][i - 2];                        // find the type if related column
                if (type_of == "string") {
                    try {
                        if (check_string_validation(tokens[i])) {
                            h = hash_string(tokens[i]);
                        }
                        else {
                            throw "ERROR: string should contain only small letters and numbers";
                        }
                    }
                    catch(char const* e) {
                        cerr << e << endl;                                // show the error if accured
                    }
                }
                else if (type_of == "timestamp") {
                    try {
                        if (check_date_validation(tokens[i])) {
                            h = hash_date(tokens[i]);
                        }
                        else {
                            throw "ERROR: Invalid type of date";
                        }
                    }
                    catch(char const* e) {
                        cerr << e << endl;                                // show the error if accured
                    }
                }
                else {
                    try {
                        if (check_int_validation(tokens[i])) {
                            h = stoll(tokens[i]);
                        }
                        else {
                            throw "ERROR: Invalid type of number";
                        }
                    }
                    catch(char const* e) {
                        cerr << e << endl;                                // show the error if accured
                    }
                }
                insert = columns[i - 2]->Insert(h);                       // insert to right column if everything goes well
                inserted_list.push_back(insert);
            }

            // fix next_fields of nodes
            for (int i = 0; i < inserted_list.size() - 1; i++) {
                inserted_list[i]->nextField = inserted_list[i + 1];
            }
            // for the last field
            inserted_list[inserted_list.size() - 1]->nextField = inserted_list[0];
        }        
    }  
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion(vector<string> tokens) {
    // tokenize the statement
    string condition = tokens[4], fields_name, operation, state;
    int pos = -1;
    pos     = condition.find("==");
    if (pos != -1) {                                     // condition is ==
        fields_name = condition.substr(0, pos);
        operation   = condition.substr(pos, 2);
        state       = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {                                 // condition is >
            fields_name = condition.substr(0, pos);
            operation   = condition.substr(pos, 1);
            state       = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");                   // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation   = condition.substr(pos, 1);
                state       = condition.substr(pos + 1);
            }
            else {                                       // condition is undefined, so break up
                return;
            }
        }
    }
    // find the column that we want to get query on it
    int col = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name) {
            col = i;
            break;
        }
    }
    // column found
    if (col != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col] == "string") {
            try {
                if (check_string_validation(state)) {     
                    h = hash_string(state);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "timestamp") {
            try {
                if (check_date_validation(state)) {
                    h = hash_date(state);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "int") {
            try {
                if (check_int_validation(state)) {
                    h = stoll(state);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        // do conditions
        if (operation == "==") {
            BTree* bt  = columns[col % columns.size()];                         // search in the right column
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);                       // find all nodes that are equal to statement
  
            for (int i = 0; i < all_equal_nodes.size(); i++) {                 // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                     // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;             // free the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                           // delete the node from column's tree
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;        // go for next field's of this entry
                    col++;
                }
            }
        }
        else if (operation == ">") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);                   // find all nodes that are greater than statement

            for (int i = 0; i < all_greater_nodes.size(); i++) {               // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                     // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true;           // free the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);                         // delete the node from column's tree
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;    // go for next field's of this entry
                    col++;
                }
            }
        }
        else if (operation == "<") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);                   // find all nodes that are smaller than statement

            for (int i = 0; i < all_smaller_nodes.size(); i++) {               // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                     // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true;           // free the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);                         // delete the node from column's tree
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;    // go for next field's of this entry
                    col++;
                }
            }
        }
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion_for_update(vector<string> tokens) {
    // tokenize the statement
    string condition = tokens[columns.size() + 3], fields_name, operation, state;
    int pos = -1;
    pos = condition.find("==");
    if (pos != -1) {                                       // condition is ==
        fields_name = condition.substr(0, pos);
        operation   = condition.substr(pos, 2);
        state       = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {                                   // condition is >
            fields_name = condition.substr(0, pos);
            operation   = condition.substr(pos, 1);
            state       = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");                     // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation   = condition.substr(pos, 1);
                state       = condition.substr(pos + 1);
            }
            else {                                         // condition is undefined, so break up
                return;
            }
        }
    }
    vector<long long int> ids;
    // find the column that we want to get query on it
    int col = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name) {
            col = i;
            break;
        }
    }
    // column found
    if (col != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col] == "string") {
            try {
                if (check_string_validation(state)) {     
                    h = hash_string(state);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "timestamp") {
            try {
                if (check_date_validation(state)) {
                    h = hash_date(state);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "int") {
            try {
                if (check_int_validation(state)) {
                    h = stoll(state);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        vector<Node*> all_nodes;                                // put all results in it
        // do conditions
        if (operation == "==") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);                            // find all nodes that are equal to statement

            for (int i = 0; i < all_equal_nodes.size(); i++) {                      // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                          // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_equal_nodes[i]->data);                    // save the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                                // delete the node from column's tree
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;             // go for next field's of this entry
                    col++;
                }
            }
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);                        // find all nodes that are greater than statement
            
            for (int i = 0; i < all_greater_nodes.size(); i++) {                    // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                          // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_greater_nodes[i]->data);                  // save the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);                              // delete the node from column's tree
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;         // go for next field's of this entry
                    col++;
                }
            }
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);                        // find all nodes that are smaller than statement
            
            for (int i = 0; i < all_smaller_nodes.size(); i++) {                    // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {                          // delete all entries of them
                    BTree* bt2 = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_smaller_nodes[i]->data);                  // save the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);                              // delete the node from column's tree
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;         // go for next field's of this entry
                    col++;
                }
            }
        }
        insert_for_update(tokens, ids, 1);                                             // go to insert the updated datas 
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion_for_update_or(vector<string> tokens) {
    // tokenize the statement
    string condition1 = tokens[columns.size() + 3], condition2 = tokens[columns.size() + 5], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    pos1     = condition1.find("==");
    // find parts of condition for both conditions
    if (pos1 != -1) {                                                // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1   = condition1.substr(pos1, 2);
        state1       = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {                                            // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1   = condition1.substr(pos1, 1);
            state1       = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");                             // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1   = condition1.substr(pos1, 1);
                state1       = condition1.substr(pos1 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {                                                // condition is ==
        fields_name2 = condition2.substr(0, pos2);
        operation2   = condition2.substr(pos2, 2);
        state2       = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {                                            // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2   = condition2.substr(pos2, 1);
            state2       = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");                             // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2   = condition2.substr(pos2, 1);
                state2       = condition2.substr(pos2 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    // find the columns that we want to get query on it
    int col1 = -1, col2 = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name1) {
            col1 = i;
            break;
        }
    }
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name2) {
            col2 = i;
            break;
        }
    }
    vector<long long int> ids;
    // column found
    if (col1 != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col1] == "string") {
            try {
                if (check_string_validation(state1)) {     
                    h = hash_string(state1);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "timestamp") {
            try {
                if (check_date_validation(state1)) {
                    h = hash_date(state1);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "int") {
            try {
                if (check_int_validation(state1)) {
                    h = stoll(state1);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do condition for first statement
        if (operation1 == "==") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);             // find all nodes that are equal to statement

            for (int i = 0; i < all_equal_nodes.size(); i++) {       // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        ids.push_back(all_equal_nodes[i]->data);     // save the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                 // delete the node
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == ">") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);         // find all nodes that are greater than statement

            for (int i = 0; i < all_greater_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        ids.push_back(all_greater_nodes[i]->data);     // save the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);               // delete the node
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == "<") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);         // find all nodes that are smaller than statement

            for (int i = 0; i < all_smaller_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        ids.push_back(all_smaller_nodes[i]->data);     // save the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);               // delete the node
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col1++;
                }
            }
        }
    }
    // column not found
    else {
        return;
    }
    // column found
    if (col2 != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col2] == "string") {
            try {
                if (check_string_validation(state2)) {     
                    h = hash_string(state2);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "timestamp") {
            try {
                if (check_date_validation(state2)) {
                    h = hash_date(state2);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "int") {
            try {
                if (check_int_validation(state2)) {
                    h = stoll(state2);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do conditions for second statement
        if (operation2 == "==") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);             // find all nodes that are equal to statement

            for (int i = 0; i < all_equal_nodes.size(); i++) {       // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        ids.push_back(all_equal_nodes[i]->data);     // save the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                 // delete the node
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == ">") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);         // find all nodes that are greater than statement

            for (int i = 0; i < all_greater_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        ids.push_back(all_greater_nodes[i]->data);     // save the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);               // delete the node
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == "<") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);         // find all nodes that are smaller than statement

            for (int i = 0; i < all_smaller_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        ids.push_back(all_smaller_nodes[i]->data);     // save the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);               // delete the node
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        insert_for_update(tokens, ids, 0);
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion_for_update_and(vector<string> tokens) {
    // tokenize the statement
    string condition1 = tokens[columns.size() + 3], condition2 = tokens[columns.size() + 5], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    pos1     = condition1.find("==");
    // find parts of condition for both conditions
    if (pos1 != -1) {                                                // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1   = condition1.substr(pos1, 2);
        state1       = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {                                            // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1   = condition1.substr(pos1, 1);
            state1       = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");                             // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1   = condition1.substr(pos1, 1);
                state1       = condition1.substr(pos1 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {                                                // condition is ==
        fields_name2 = condition2.substr(0, pos2);
        operation2   = condition2.substr(pos2, 2);
        state2       = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {                                            // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2   = condition2.substr(pos2, 1);
            state2       = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");                             // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2   = condition2.substr(pos2, 1);
                state2       = condition2.substr(pos2 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    // find the columns that we want to get query on it
    int col1 = -1, col2 = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name1) {
            col1 = i;
            break;
        }
    }
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name2) {
            col2 = i;
            break;
        }
    }
    vector<long long int> ids;
    // columns found
    if (col1 != -1 && col2 != -1) {
        vector<Node*> all_nodes1, all_nodes2;
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col1] == "string") {
            try {
                if (check_string_validation(state1)) {     
                    h = hash_string(state1);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "timestamp") {
            try {
                if (check_date_validation(state1)) {
                    h = hash_date(state1);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "int") {
            try {
                if (check_int_validation(state1)) {
                    h = stoll(state1);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do conditions
        if (operation1 == "==") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_equal(rt, h, all_nodes1);                  // find all nodes that are equal to statement
        }
        else if (operation1 == ">") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_greater(rt, h, all_nodes1);                // find all nodes that are greater than statement
        }
        else if (operation1 == "<") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            // vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_nodes1);                // find all nodes that are smaller than statement
        }
        // the second field
        if (fields[1][col2] == "string") {
            try {
                if (check_string_validation(state2)) {     
                    h = hash_string(state2);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "timestamp") {
            try {
                if (check_date_validation(state2)) {
                    h = hash_date(state2);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "int") {
            try {
                if (check_int_validation(state2)) {
                    h = stoll(state2);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // check that answer of first condition have second condition, it do then delete them
        for (int i = 0; i < all_nodes1.size(); i++) {
            Node* check = all_nodes1[i];
            int move    = col1, counter = 0;
            while ((move + counter) % columns.size() != col2) {
                check = check->nextField;
                counter++;
            }
            if (operation2 == "==") {
                if (check->data == h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
            else if (operation2 == ">") {
                if (check->data > h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
            else if (operation2 == "<") {
                if (check->data < h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
        }
        for (int i = 0; i < all_nodes2.size(); i++) {                // for all nodes that had the condition
            for (int j = 0; j < columns.size(); j++) {               // delete all entries of them
                BTree* bt2 = columns[col1 % columns.size()];
                if (fields[0][col1 % columns.size()] == "id") {
                    ids.push_back(all_nodes2[i]->data);              // save the deleted nodes id
                }
                bt2->Delete(all_nodes2[i]);                          // delete the node
                all_nodes2[i] = all_nodes2[i]->nextField;
                col1++;
            }
        }
        insert_for_update(tokens, ids, 0);
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion_or(vector<string> tokens) {
    string condition1 = tokens[4], condition2 = tokens[6], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    pos1     = condition1.find("==");
    // find parts of condition for both conditions
    if (pos1 != -1) {                                                // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1   = condition1.substr(pos1, 2);
        state1       = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {                                            // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1   = condition1.substr(pos1, 1);
            state1       = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");                             // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1   = condition1.substr(pos1, 1);
                state1       = condition1.substr(pos1 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {                                                // condition is ==
        fields_name2 = condition2.substr(0, pos2);
        operation2   = condition2.substr(pos2, 2);
        state2       = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {                                            // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2   = condition2.substr(pos2, 1);
            state2       = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");                             // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2   = condition2.substr(pos2, 1);
                state2       = condition2.substr(pos2 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    // find the columns that we want to get query on it
    int col1 = -1, col2 = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name1) {
            col1 = i;
            break;
        }
    }
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name2) {
            col2 = i;
            break;
        }
    }
    // column found
    if (col1 != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col1] == "string") {
            try {
                if (check_string_validation(state1)) {     
                    h = hash_string(state1);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "timestamp") {
            try {
                if (check_date_validation(state1)) {
                    h = hash_date(state1);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "int") {
            try {
                if (check_int_validation(state1)) {
                    h = stoll(state1);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do condition for first statement
        if (operation1 == "==") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);             // find all nodes that are equal to statement

            for (int i = 0; i < all_equal_nodes.size(); i++) {       // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;   // free the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                 // delete the node
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == ">") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);         // find all nodes that are greater than statement

            for (int i = 0; i < all_greater_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true; // free the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);               // delete the node
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == "<") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);         // find all nodes that are smaller than statement

            for (int i = 0; i < all_smaller_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true; // free the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);               // delete the node
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col1++;
                }
            }
        }
    }
    // column not found
    else {
        return;
    }
    // column found
    if (col2 != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col2] == "string") {
            try {
                if (check_string_validation(state2)) {     
                    h = hash_string(state2);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "timestamp") {
            try {
                if (check_date_validation(state2)) {
                    h = hash_date(state2);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "int") {
            try {
                if (check_int_validation(state2)) {
                    h = stoll(state2);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do conditions for second statement
        if (operation2 == "==") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            bt->check_for_equal(rt, h, all_equal_nodes);             // find all nodes that are equal to statement

            for (int i = 0; i < all_equal_nodes.size(); i++) {       // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;   // free the deleted nodes id
                    }
                    bt2->Delete(all_equal_nodes[i]);                 // delete the node
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == ">") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            bt->check_for_greater(rt, h, all_greater_nodes);         // find all nodes that are greater than statement

            for (int i = 0; i < all_greater_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true; // free the deleted nodes id
                    }

                    bt2->Delete(all_greater_nodes[i]);               // delete the node
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == "<") {
            BTree* bt  = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_smaller_nodes);         // find all nodes that are smaller than statement

            for (int i = 0; i < all_smaller_nodes.size(); i++) {     // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {           // delete all entries of them
                    BTree* bt2 = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true; // free the deleted nodes id
                    }
                    bt2->Delete(all_smaller_nodes[i]);               // delete the node
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col2++;
                }
            }
        }
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::deletion_and(vector<string> tokens) {
    string condition1 = tokens[4], condition2 = tokens[6], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    // find both of conditions
    pos1 = condition1.find("==");
    if (pos1 != -1) {                                                // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1   = condition1.substr(pos1, 2);
        state1       = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {                                            // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1   = condition1.substr(pos1, 1);
            state1       = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");                             // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1   = condition1.substr(pos1, 1);
                state1       = condition1.substr(pos1 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {                                                // condition is ==
        fields_name2 = condition2.substr(0, pos2);
        operation2   = condition2.substr(pos2, 2);
        state2       = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {                                            // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2   = condition2.substr(pos2, 1);
            state2       = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");                             // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2   = condition2.substr(pos2, 1);
                state2       = condition2.substr(pos2 + 1);
            }
            else {                                                   // condition is undefined, so break up
                return;
            }
        }
    }
    // find the columns that we want to get query on it
    int col1 = -1, col2 = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name1) {
            col1 = i;
            break;
        }
    }
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name2) {
            col2 = i;
            break;
        }
    }
    // columns found
    if (col1 != -1 && col2 != -1) {
        vector<Node*> all_nodes1, all_nodes2;
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col1] == "string") {
            try {
                if (check_string_validation(state1)) {     
                    h = hash_string(state1);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "timestamp") {
            try {
                if (check_date_validation(state1)) {
                    h = hash_date(state1);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col1] == "int") {
            try {
                if (check_int_validation(state1)) {
                    h = stoll(state1);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // do conditions
        if (operation1 == "==") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_equal(rt, h, all_nodes1);                  // find all nodes that are equal to statement
        }
        else if (operation1 == ">") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_greater(rt, h, all_nodes1);                // find all nodes that are greater than statement
        }
        else if (operation1 == "<") {
            BTree* bt  = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            // vector<Node*> all_smaller_nodes;
            bt->check_for_smaller(rt, h, all_nodes1);                // find all nodes that are smaller than statement
        }
        // the second field
        if (fields[1][col2] == "string") {
            try {
                if (check_string_validation(state2)) {     
                    h = hash_string(state2);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "timestamp") {
            try {
                if (check_date_validation(state2)) {
                    h = hash_date(state2);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        else if (fields[1][col2] == "int") {
            try {
                if (check_int_validation(state2)) {
                    h = stoll(state2);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                                   // show the error if accured
            }
        }
        // check that answer of first condition have second condition, it do then delete them
        for (int i = 0; i < all_nodes1.size(); i++) {
            Node* check = all_nodes1[i];
            int move    = col1, counter = 0;
            while ((move + counter) % columns.size() != col2) {
                check = check->nextField;
                counter++;
            }
            if (operation2 == "==") {
                if (check->data == h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
            else if (operation2 == ">") {
                if (check->data > h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
            else if (operation2 == "<") {
                if (check->data < h) {
                    all_nodes2.push_back(all_nodes1[i]);
                }
            }
        }
        for (int i = 0; i < all_nodes2.size(); i++) {                // for all nodes that had the condition
            for (int j = 0; j < columns.size(); j++) {               // delete all entries of them
                BTree* bt2 = columns[col1 % columns.size()];
                if (fields[0][col1 % columns.size()] == "id") {
                    id_table[all_nodes2[i]->data] = true;            // free the deleted nodes id
                }
                bt2->Delete(all_nodes2[i]);                          // delete the node
                all_nodes2[i] = all_nodes2[i]->nextField;
                col1++;
            }
        }
    }
    // column not found
    else {
        return;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::update(vector<string> tokens) {
    deletion_for_update(tokens);             // delete the entries with guven condition and then updated data
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::update_or(vector<string> tokens) {
    deletion_for_update_or(tokens);             // delete the entries with guven condition and then updated data
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Table::update_and(vector<string> tokens) {
    deletion_for_update_and(tokens);             // delete the entries with guven condition and then updated data
}
///////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<string>> Table::select(vector<string> tokens, int table_ind) {
    vector<int> selected_columns;                               // columns we want their entries
    int num_sel = table_ind - 2;                                // number of columns we want their data for result
    if (tokens[1] == "*") {                                     // * = all fields
        num_sel = columns.size();
        for (int i = 0; i < columns.size(); i++) {
            selected_columns.push_back(i);                      // choose all columns
        }
    }
    else {
        for (int i = 0; i < num_sel; i++) {                     // for number of fields we want
            for (int j = 0; j < columns.size(); j++) {          // check all columns
                if (fields[0][j] == tokens[i + 1]) {            // if the field mentioned choose it
                    selected_columns.push_back(j);
                    break;
                }
            }
        }
    }
    int pos_tok      =  table_ind + 2;                          // find the index of condition
    string condition = tokens[pos_tok], fields_name, operation, state;
    int pos          = -1;
    // find the condition phrase
    pos = condition.find("==");
    if (pos != -1) {                                            // condition is ==
        fields_name = condition.substr(0, pos);
        operation   = condition.substr(pos, 2);
        state       = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {                                        // condition is >
            fields_name = condition.substr(0, pos);
            operation   = condition.substr(pos, 1);
            state       = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");                          // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation   = condition.substr(pos, 1);
                state       = condition.substr(pos + 1);
            }
            else {                                              // condition is undefined, so break up
                return {{}};
            }
        }
    }
    // find the column that we want to get query on it
    int col = -1;
    for (int i = 0; i < fields[0].size(); i++) {
        if (fields[0][i] == fields_name) {
            col = i;
            break;
        }
    }
    // column found
    if (col != -1) {
        long long int h;
        // hash the statement base on type of column
        if (fields[1][col] == "string") {
            try {
                if (check_string_validation(state)) {     
                    h = hash_string(state);
                }
                else {
                    throw "ERROR: string should contain only small letters and numbers";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "timestamp") {
            try {
                if (check_date_validation(state)) {
                    h = hash_date(state);
                }
                else {
                    throw "ERROR: Invalid type of date";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        else if (fields[1][col] == "int") {
            try {
                if (check_int_validation(state)) {
                    h = stoll(state);
                }
                else {
                    throw "ERROR: Invalid type of number";
                }
            }
            catch(char const* e) {
                cerr << e << endl;                              // show the error if accured
            }
        }
        vector<Node*> all_nodes;                                // save all nodes that have condition
        // do conditions
        if (operation == "==") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_equal(rt, h, all_nodes);              // find all nodes that are equal to statement
        }
        else if (operation == ">") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_greater(rt, h, all_nodes);            // find all nodes that are greater than statement
        }
        else if (operation == "<") {
            BTree* bt  = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            bt->check_for_smaller(rt, h, all_nodes);            // find all nodes that are equal to statement
        }
        
        vector<vector<string>> answers;                         // put final answer into it
        vector<Node*> sorted1;                                  // sort the found nodes base on id
        for (int i = 0; i < all_nodes.size(); i++) {
            Node* move  = all_nodes[i];
            int cur_col = col;
            
            while (cur_col % columns.size() != 0) {             // go untill get the id field
                move = move->nextField;
                cur_col++;
            }
            sorted1.push_back(move);
        }
        // sort node of answers base on their id
        for (int i = 0; i < sorted1.size(); i++) {
            for (int j = i + 1; j < sorted1.size(); j++) {
                if (sorted1[i]->data > sorted1[j]->data) {
                    Node* temp = sorted1[i];
                    sorted1[i] = sorted1[j];
                    sorted1[j] = temp;
                }
            }
        }
        // only chosse the requested fields of answers
        for (int i = 0; i < sorted1.size(); i++) {
            answers.push_back({});
            Node* move = sorted1[i];

            for (int j = 0; j < columns.size(); j++) {
                for (int k = 0; k < selected_columns.size(); k++) {
                    if (selected_columns[k] == j) {                       // got to one of selected columns
                        if (fields[1][j] == "string") {
                            string dehash = dehash_string(move->data);    // convert to it's string
                            answers[i].push_back(dehash);
                            break;
                        }
                        else if (fields[1][j] == "timestamp") {
                            string dehash = dehash_date(move->data);      // convert to it's date
                            answers[i].push_back(dehash);
                            break;
                        }
                        else {
                            answers[i].push_back(to_string(move->data));
                            break;
                        }
                        
                    }
                }
                move = move->nextField;                                   // go and check next field
            }
        }
        return answers;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    
    string input;
    vector<vector<string>> all_select_queries;
    vector <Table*> all_tables;                                                     // save all tables in it
    int n;
    cin >> n;
    cin.ignore();
    for (int q = 0; q < n; q++) {
        getline(cin, input);                                                        // get the line of input
        vector<string> tokens = parse_query(input);                                 // tokenize the input line

        // turn tokens to sql code
        string operation      = tokens[0];
        // first token shows operation to call the right method
        if (operation == "CREATE") {
            string table = tokens[1];                                               // second token should be TABLE
            if (table == "TABLE") {
                string table_name = tokens[2];                                      // third token should be name of table we wanna create
                all_tables.push_back(new Table(table_name));                        // add object of table to all table objects list
                Table* table_obj  = all_tables[all_tables.size() - 1];
                table_obj->set_id(all_tables.size());

                // each table should contain column id
                table_obj->create_filed("id", "int");
                // add other fields of table to it
                for (int i = 3; i < tokens.size(); i+=2) {
                    table_obj->create_filed(tokens[i], tokens[i + 1]);
                }
            }
        }
        else if (operation == "INSERT") {
            string into = tokens[1];                                                // second token should be INTO
            if (into == "INTO") {
                string table_name = tokens[2];                                      // third token should be name of table we wanna insert to
                // find the table object
                Table* table_obj  = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                    // cout << "Table not found" << endl;
                }
                // table found
                else {
                    table_obj->insertion(tokens);
                }
            }
        }
        else if (operation == "DELETE") {
            string from = tokens[1];                                                // second token should be FROM
            if (from == "FROM") {
                string table_name = tokens[2];                                      // third token should be name of table we wanna delete from
                // find the table object
                Table* table_obj  = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                    // cout << "Table not found" << endl;
                }
                // table found
                else {
                    // delete with single condition
                    if (tokens.size() == 5) {                                       // single condition delete
                        table_obj->deletion(tokens);
                    }
                    else if (tokens.size() == 7) {                                  // double condition delete with or
                        if (tokens[5] == "|") 
                            table_obj->deletion_or(tokens);
                        else if (tokens[5] == "&")                                  // double condition delete with and
                            table_obj->deletion_and(tokens);
                    }
                }
            }           
        }
        else if (operation == "UPDATE") {
            string table_name = tokens[1];                                          // second token should be name of table we wanna update 
            // find the table object
            Table* table_obj  = NULL;
            for (int i = 0; i < all_tables.size(); i++) {
                if (all_tables[i]->get_name() == table_name) {
                    table_obj = all_tables[i];
                    break;
                }
            }
            // the table not exists
            if (!table_obj) {
                // cout << "Table not found" << endl;
            }
            // table found
            else {
                if (tokens[tokens.size() - 2] == "|") {                                       // single condition delete
                    table_obj->update_or(tokens);
                }
                else if (tokens[tokens.size() - 2] == "&") {                                  // double condition delete with or
                    table_obj->update_and(tokens);
                }
                else {
                    table_obj->update(tokens);
                }
            }
        }
        else if (operation == "SELECT") {
            int table_ind = -1;
            for (int i = 0; i < tokens.size(); i++) {
                if (tokens[i] == "FROM") {
                    table_ind = i + 1;
                    break;
                }
            }
            if (table_ind != -1) {
                string table_name = tokens[table_ind];                              // get name of table we wanna select from
                // find the table object
                Table* table_obj  = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                    // cout << "Table not found" << endl;
                }
                // table found
                else {
                    vector<vector<string>> all_selects = table_obj->select(tokens, table_ind);
                    for (int k = 0; k < all_selects.size(); k++) {                  // add result of query to all thing we should show at last
                        all_select_queries.push_back(all_selects[k]);
                    }
                }
            }
        }
    }
    // show all answers
    for (int i = 0; i < all_select_queries.size(); i++) {
        for (int j = 0; j < all_select_queries[i].size(); j++) {
            cout << all_select_queries[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}

