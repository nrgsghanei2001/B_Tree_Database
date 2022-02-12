#include <iostream>
#include <cmath>
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