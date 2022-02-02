#include<iostream>
#include<vector>
#include<cmath>
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

        BTNode(int);
        BTNode* find_insert_place(int, BTNode*, int, BTNode*);
        void split_nodes(BTNode*, int);
        void traverse(int);
        void delete_node(int, int);
        Node* predecessor(int);
        Node* successor(int);
};
/////////////////////////////////////////////////////
// constructor
BTNode::BTNode(int m) {
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
        while (i != 0 && key[i - 1]->data > data) {
            key[i] = key[i - 1];
            i--;
        }
        // initializing the new key
        Node* new_key = new Node;
        new_key->data = data;
        new_key->nextField = NULL;
        new_key->self = curr;
        key[i] = new_key;     // put the key in the right place
        num_keys++;
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
///////////////////////////////////////////////////////////////////////
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
        for (int i = ceil(float(t) / 2); i <= t; i++) {
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
        child[ri_counter+1] = child[ri_counter];
        ri_counter--;
    }
    // put right child in the correct place
    child[ri_counter + 1] = right;
    // move the median up
    int mid_counter = t - 1;
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
    curr->num_keys--;       // one moved up so decrease it
    
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
void BTNode::delete_node(int k, int t) {
    // find the parent node place or exact place of accurance
    int key_counter = 0;
    // do untill getting the key that is greater than k
    while (key_counter < num_keys && key[key_counter]->data < k) {
        key_counter++;
    }
    // key_counter is the next place we should go
    // case 1: k is exactly in this node
    if (key[key_counter]->data == k && key_counter < num_keys) {
        // case 1-1: delete from leaf
        if (is_leaf) {
            // just move next keys one step backward
            for (int i = key_counter + 1; i < num_keys; i++) {
                key[i - 1] = key[i];
            }
            // reduce the number of keys by 1
            num_keys--;
        }
        // case 1-2: delete from non-leaf node
        else {
            // case 1-2-1: this key's child has at least t/2 keys
            if (child[key_counter]->num_keys >= t / 2) {
                Node* pre = predecessor(key_counter);
                // replace the predecessor with deleted key
                key[key_counter] = pre;
                // recursively remove the predesessor
                child[key_counter]->delete_node(pre->data, t);
            } 
            // case 1-2-2: this key's child has not enough children so test his right brother
            else if (child[key_counter + 1]->num_keys >= t / 2) {
                Node* suc = successor(key_counter);
                // replace the successor with deleted key
                key[key_counter] = suc;
                // recursively remove the successor
                child[key_counter + 1]->delete_node(suc->data, t);
            }
            // case 1-2-3: non of them have enough children so merge 2 children
            else {
                BTNode* lchild = child[key_counter];
                BTNode* rchild = child[key_counter + 1];
                // put the key in rchild
                lchild->key[(t / 2) - 1] = key[key_counter];
                // move right child keys and children to left child 
                for (int i = 0; i < lchild->num_keys; i++) {
                    lchild->key[i + (t / 2)] = rchild->key[i];
                    // if it has any children pass them to left child
                    if (lchild->is_leaf == false) {
                        lchild->child[i + (t / 2)] = rchild->child[i];
                    }
                }
                // move keys 1 step and children 2 step backward
                for (int i = key_counter + 1; i < num_keys; i++) {
                    key[i - 1] = key[i];
                    child[i] = child[i + 1];
                }

                // number of keys in left child increased 
                lchild->num_keys += rchild->num_keys + 1;
                // number of keys in this node decreased
                num_keys--;
                // free the memory of right child after merging
                delete(rchild);
                // recursively remove the key that is in child now
                child[key_counter]->delete_node(key_counter, t);
            }
        }
    }
    // case 2: k is not accured in this node
    else {
        // case 2-1: we are at leaf node and there is not any chilren so the key was not found
        if (is_leaf) {
            cout << "sorry! there is not such a key in this tree.";
            return;
        }
        // case 2-2: the key is in subtrees of this node
        else {
            // case 2-2-1: key is in last keys child and the child has more than allowed keys
            // check if it is the last child
            int last_child = 0;
            if (key_counter == num_keys) {
                last_child = 1;
            }
            if (child[key_counter]->num_keys < t / 2) {
                // case 2-2-1-1: borrow key from left brother
                if (key_counter != 0 && child[key_counter - 1]->num_keys >= t / 2) {
                    BTNode* rrchild = child[key_counter];
                    BTNode* llchild = child[key_counter - 1];
                    
                    // move children and children of it forward
                    for (int i = rrchild->num_keys - 1; i >= 0; i--) {
                        rrchild->key[i + 1] = rrchild->key[i];
                        // it has children
                        if (rrchild->is_leaf == false) {
                            rrchild->child[i + 1] = rrchild->child[i];
                        }
                    }
                    // pass the children
                    rrchild->key[0] = key[key_counter - 1];
                    if (rrchild->is_leaf == false) {
                        rrchild->child[0] = llchild->child[llchild->num_keys];
                    }
                    // move the right child's key up
                    key[key_counter - 1] = llchild->key[llchild->num_keys - 1];
                    // update number of keys
                    rrchild->num_keys++;
                    llchild->num_keys--;
                }
                // case 2-2-1-2: borrow key from right brother
                else if (key_counter != num_keys && child[key_counter + 1]->num_keys >= t / 2) {
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
                    // move keys and children of right child backward
                    for (int i = 1; i < rrchild->num_keys; i++) {
                        rrchild->key[i - 1] = rrchild->key[i];
                    }
                    // if it has any children
                    if (rrchild->is_leaf == false) {
                        for (int i = 0; i < rrchild->num_keys; i++) {
                            rrchild->child[i] = rrchild->child[i + 1];
                        }
                    }
                    
                    // update number of keys
                    rrchild->num_keys--;
                    llchild->num_keys++;
                }
                // case 2-2-1-3: we can't borrow from brothers so merge with next child
                else if (key_counter != num_keys) {
                    BTNode* lchild = child[key_counter];
                    BTNode* rchild = child[key_counter + 1];
                    // put the key in rchild
                    lchild->key[(t / 2) - 1] = key[key_counter];
                    // move right child keys and children to left child 
                    for (int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                    }
                    // move keys 1 step and children 2 step backward
                    for (int i = key_counter + 1; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
                    }

                    // number of keys in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of keys in this node decreased
                    num_keys--;
                    // free the memory of right child after merging
                    delete(rchild);
                }
                // case 2-2-1-4: we can't borrow from brothers so merge with last child
                else {
                    BTNode* lchild = child[key_counter - 1];
                    BTNode* rchild = child[key_counter];
                    // put the key in rchild
                    lchild->key[(t / 2) - 1] = key[key_counter - 1];
                    // move right child keys and children to left child 
                    for (int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                    }
                    // move keys 1 step and children 2 step backward
                    for (int i = key_counter; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
                    }

                    // number of keys in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of keys in this node decreased
                    num_keys--;
                    // free the memory of right child after merging
                    delete(rchild);
                }
            }
            // case 2-2-2: children are merged
            if (last_child && key_counter > num_keys) {
                // recursively delete
                child[key_counter - 1]->delete_node(k, t);
            }
            // case 2-2-3: children are not merged or key_counter does not increased
            else {
                child[key_counter]->delete_node(k, t);
            }
        }
    }
    return;
}
////////////////////////////////////////////////
Node* BTNode::predecessor(int key_counter) {
    BTNode* node = child[key_counter];
    // get the right most node
    while (node->is_leaf == false) {
        node = node->child[node->num_keys];
    }

    return node->key[node->num_keys-1];
    
}
/////////////////////////////////////////////////////
Node* BTNode::successor(int key_counter) {
    BTNode* node = child[key_counter + 1];
    // get the left most node
    while (node->is_leaf == false) {
        node = node->child[0];
    }

    return node->key[0];
    
}
///////////////////////////////////////////////////////
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
///////////////////////////////////////////////////
int main() {
    int t = 3;
    int ins, del;
    cin >> ins >> del;
    BTree tree(t);
    for (int i = 0; i < ins; i++) {
        int w;
        cin >> w;
        tree.Insert(w);
    }
    tree.traverse();
    for (int i = 0; i < del; i++) {
        int w;
        cin >> w;
        tree.Delete(w);
        tree.traverse();
    }
}