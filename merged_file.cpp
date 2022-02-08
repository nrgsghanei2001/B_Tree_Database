#include<iostream>
#include<vector>
#include<cmath>
using namespace std;

class BTNode;
// Class Node for each elements inside elements of B-tree
class Node {
    public:
        long long int data;
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
        long long int num_keys;
        int maxx;

        BTNode(long long int);
        BTNode* find_insert_place(long long int, BTNode*, long long int, BTNode*, Node* &);
        void split_nodes(BTNode*, long long int);
        void traverse(long long int);
        void delete_node(long long int, long long int);
        void delete_node2(Node*, long long int);
        void deletion(Node*, long long int);
        void merge(long long int, long long int);
        Node* predecessor(long long int);
        Node* successor(long long int);
        Node* search_node(long long int);
        Node* find_greaters(long long int);
        Node* find_smallers(long long int);
        Node* insertNonFull(long long int k);
        void splitChild(int, BTNode*);
};
/////////////////////////////////////////////////////
// constructor
BTNode::BTNode(long long int m) {
    maxx = m;
    key = new Node*[m];
    is_leaf = true;
    num_keys = 0;
    child = new BTNode*[m + 1];
    for (long long int i = 0; i < m; i++) {
        key[i] = NULL;
    }
    for (long long int i = 0; i < m + 1; i++) {
        child[i] = NULL;
    }
}
/////////////////////////////////////////////////////
BTNode* BTNode::find_insert_place(long long int data, BTNode* curr, long long int t, BTNode* root, Node* & new_key) {
    // case 1: we are at leaf node
    if (is_leaf) {
        long long int i = t;
        // find the first null place in key
        while (key[i - 1] == NULL) {
            i--;
        }
        // data is bigger than cuurent key, pass untill getting the right place
        while (i != 0 && key[i - 1]->data > data) {
            key[i] = key[i - 1];
            i--;
        }
        // initializing the new key
        // Node* new_key = new Node;
        new_key->data = data;
        new_key->nextField = NULL;
        new_key->self = curr;
        key[i] = new_key;     // put the key in the right place
        num_keys++;
    }
    // case 2: we are not at leaf
    else {
        long long int i = 0;
        // find the right child to insert to or get out if node is full
        while (i < num_keys && key[i]->data < data) {
            i++;
        }
        // the right child found, go recusrively to find the real place at the leaf
        child[i]->find_insert_place(data, this, t, root, new_key);   
    }
    // the current node is full
    if (num_keys == t) {
        // we are at root node
        if (this == root) {
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
void BTNode::split_nodes(BTNode* curr, long long int t) {
    // create new node for right child
    BTNode* right = new BTNode(t);
    long long int r_counter = 0;
    long long int mid = (t - 1) / 2;  // index of median to put it up
    long long int number_of_keys = curr->num_keys;   // number of key in full node
    Node* median = curr->key[mid];   // median of key
    
    for (long long int i = mid + 1; i < number_of_keys; i++) {
        right->key[r_counter] = curr->key[i];        // put key after median to right child
        r_counter++;
        curr->num_keys--;      // number of key decreases 
        right->num_keys++;     // number of key of right child increases
    }
    // current node has childred
    long long int ch_counter = 0;
    if (!curr->is_leaf) {
        for (long long int i = ceil(float(t) / 2); i <= t; i++) {
            right->child[ch_counter] = curr->child[i];     // pass children of current node to new right child
            ch_counter++;
        }
        right->is_leaf = curr->is_leaf;
    }
    // do this for owning chidren and non owning chidren nodes
    // allocate place for right children
    long long int ri_counter = t - 1;
    // continue untill you get to current node
    while (child[ri_counter] != curr) {
        child[ri_counter+1] = child[ri_counter];
        ri_counter--;
    }
    // put right child in the correct place
    child[ri_counter + 1] = right;
    // move the median up
    long long int mid_counter = t - 1;
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
void BTNode::traverse(long long int tab) {
    long long int i;
    string s;
 
    // Print 'tab' number of tabs
    for (long long int j = 0; j < tab; j++) {
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
void BTNode::merge(long long int key_counter, long long int t) {
  BTNode *lchild = child[key_counter];
  BTNode *rchild = child[key_counter + 1];

  lchild->key[(t / 2) - 1] = key[key_counter];

  for (int i = 0; i < rchild->num_keys; ++i)
    lchild->key[i + (t / 2)] = rchild->key[i];

  if (!lchild->is_leaf) {
    for (int i = 0; i <= rchild->num_keys; ++i)
      lchild->child[i + (t / 2)] = rchild->child[i];
  }

  for (int i = key_counter + 1; i < num_keys; ++i)
    key[i - 1] = key[i];

  for (int i = key_counter + 2; i <= num_keys; ++i)
    child[i - 1] = child[i];

  lchild->num_keys += rchild->num_keys + 1;
  key[num_keys - 1] = NULL;
  num_keys--;

  delete (rchild);
  return;
}
////////////////////////////////////////////////
void BTNode::deletion(Node* k, long long int t) {
  int key_counter = 0;
  while (key_counter < num_keys && key[key_counter]->data < k->data)
    ++key_counter;

  if (key_counter < num_keys && key[key_counter]->data == k->data && key[key_counter] != k) {
        key_counter++;
    } 

  if (key_counter < num_keys && key[key_counter] == k) {
    if (is_leaf) {
        for (int i = key_counter + 1; i < num_keys; ++i)
            key[i - 1] = key[i];
        key[num_keys - 1] = NULL;
        num_keys--;
    }
    else {
        Node* k = key[key_counter];

        if (child[key_counter]->num_keys >= (t / 2)) {
            Node* pred = predecessor(key_counter);
            key[key_counter] = pred;
            child[key_counter]->deletion(pred, t);
        }

        else if (child[key_counter + 1]->num_keys >= (t / 2)) {
            Node* succ = successor(key_counter);
            key[key_counter] = succ;
            child[key_counter + 1]->deletion(succ, t);
        }

        else {
            merge(key_counter, t);
            child[key_counter]->deletion(k, t);
        }
    }
  }
  else {
    if (is_leaf) {
      return;
    }

    bool flag = ((key_counter == num_keys) ? true : false);

    if (child[key_counter]->num_keys < (t / 2)) {
        if (key_counter != 0 && child[key_counter - 1]->num_keys >= (t / 2)) {
            BTNode *lchild = child[key_counter];
            BTNode *rchild = child[key_counter - 1];

            for (int i = lchild->num_keys - 1; i >= 0; --i)
                lchild->key[i + 1] = lchild->key[i];

            if (!lchild->is_leaf) {
                for (int i = lchild->num_keys; i >= 0; --i)
                lchild->child[i + 1] = lchild->child[i];
            }

            lchild->key[0] = key[key_counter - 1];

            if (!lchild->is_leaf)
                lchild->child[0] = rchild->child[rchild->num_keys];

            key[key_counter - 1] = rchild->key[rchild->num_keys - 1];

            lchild->num_keys += 1;
            rchild->num_keys -= 1;
        }

        else if (key_counter != num_keys && child[key_counter + 1]->num_keys >= (t / 2)) {
            BTNode *lchild = child[key_counter];
            BTNode *rchild = child[key_counter + 1];

            lchild->key[(lchild->num_keys)] = key[key_counter];

            if (!(lchild->is_leaf))
                lchild->child[(lchild->num_keys) + 1] = rchild->child[0];

            key[key_counter] = rchild->key[0];

            for (int i = 1; i < rchild->num_keys; ++i)
                rchild->key[i - 1] = rchild->key[i];

            if (!rchild->is_leaf) {
                for (int i = 1; i <= rchild->num_keys; ++i)
                rchild->child[i - 1] = rchild->child[i];
            }

            lchild->num_keys += 1;
            rchild->num_keys -= 1;
        }

        else {
            if (key_counter != num_keys)
            merge(key_counter, t);
            else
            merge(key_counter - 1, t);
        }
    }

    if (flag && key_counter > num_keys)
      child[key_counter - 1]->deletion(k, t);
    else
      child[key_counter]->deletion(k, t);
  }
  return;
}
////////////////////////////////////////////////
void BTNode::delete_node(long long int k, long long int t) {
    // find the parent node place or exact place of accurance
    long long int key_counter = 0;
    // do untill getting the key that is greater than k
    while (key_counter < num_keys && key[key_counter]->data < k) {
        key_counter++;
    }
    // key_counter is the next place we should go
    // case 1: k is exactly in this node
    if (key_counter < num_keys && key[key_counter]->data == k) {
        // case 1-1: delete from leaf
        if (is_leaf) {
            // just move next key one step backward
            for (long long int i = key_counter + 1; i < num_keys; i++) {
                key[i - 1] = key[i];
            }
            // reduce the number of key by 1
            // key[num_keys - 1] = NULL;
            num_keys--;
        }
        // case 1-2: delete from non-leaf node
        else {
            // case 1-2-1: this key's child has at least t/2 key
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
                // move right child key and children to left child 
                for (long long int i = 0; i < rchild->num_keys; i++) {
                    lchild->key[i + (t / 2)] = rchild->key[i];
                    // if it has any children pass them to left child
                    if (lchild->is_leaf == false) {
                        lchild->child[i + (t / 2)] = rchild->child[i];
                    }
                    if (lchild->is_leaf == false && i == rchild->num_keys - 1) {
                        lchild->child[i + 1 + (t / 2)] = rchild->child[i + 1];
                    }
                }
                
                // move key 1 step and children 2 step backward
                for (long long int i = key_counter + 1; i < num_keys; i++) {
                    key[i - 1] = key[i];
                    child[i] = child[i + 1];
                }

                // number of key in left child increased 
                lchild->num_keys += rchild->num_keys + 1;
                // number of key in this node decreased
                num_keys--;
                // free the memory of right child after merging
                delete(rchild);
                // recursively remove the key that is in child now
                child[key_counter]->delete_node(k, t);
            }
        }
    }
    // case 2: k is not accured in this node
    else {
        // case 2-1: we are at leaf node and there is not any chilren so the key was not found
        if (is_leaf) {
            return;
        }
        // case 2-2: the key is in subtrees of this node
        else {
            // case 2-2-1: key is in last key child and the child has more than allowed key
            // check if it is the last child
            long long int last_child = 0;
            if (key_counter == num_keys) {
                last_child = 1;
            }
            if (child[key_counter]->num_keys < t / 2) {
                // case 2-2-1-1: borrow key from left brother
                if (key_counter != 0 && child[key_counter - 1]->num_keys >= t / 2) {
                    BTNode* rrchild = child[key_counter];
                    BTNode* llchild = child[key_counter - 1];
                    
                    // move children and children of it forward
                    for (long long int i = rrchild->num_keys - 1; i >= 0; i--) {
                        rrchild->key[i + 1] = rrchild->key[i];
                        // it has children
                        if (rrchild->is_leaf == false) {
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
                    // move key and children of right child backward
                    for (long long int i = 1; i < rrchild->num_keys; i++) {
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
                // case 2-2-1-3: we can't borrow from brothers so merge with next child
                else if (key_counter != num_keys) {
                    BTNode* lchild = child[key_counter];
                    BTNode* rchild = child[key_counter + 1];
                    // put the key in rchild
                    lchild->key[(t / 2) - 1] = key[key_counter];
                    // move right child key and children to left child 
                    for (long long int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (long long int i = key_counter + 1; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
                    }

                    // number of key in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of key in this node decreased
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
                    // move right child key and children to left child 
                    for (long long int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (long long int i = key_counter; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
                    }

                    // number of key in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of key in this node decreased
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
//////////////////////////////////////////////////////////////////////
void BTNode::delete_node2(Node* k, long long int t) {
    // find the parent node place or exact place of accurance
    long long int key_counter = 0;
    // do untill getting the key that is greater than k
    while (key_counter < num_keys && key[key_counter]->data < k->data) {
        key_counter++;
    }
    // key_counter is the next place we should go
    if (key_counter < num_keys && key[key_counter]->data == k->data && key[key_counter] != k) {
        key_counter++;
    } 
    // case 1: k is exactly in this node
    if (key_counter < num_keys && key[key_counter] == k) {
        // case 1-1: delete from leaf
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
        // case 1-2: delete from non-leaf node
        else {
            Node* k2 = key[key_counter];
            // case 1-2-1: this key's child has at least t/2 key
            if (child[key_counter]->num_keys >= t / 2) {
                Node* pre = predecessor(key_counter);
                // replace the predecessor with deleted key
                key[key_counter] = pre;
                // recursively remove the predesessor
                child[key_counter]->delete_node2(pre, t);
            } 
            // case 1-2-2: this key's child has not enough children so test his right brother
            else if (child[key_counter + 1]->num_keys >= t / 2) {
                Node* suc = successor(key_counter);
                // replace the successor with deleted key
                key[key_counter] = suc;
                // recursively remove the successor
                child[key_counter + 1]->delete_node2(suc, t);
            }
            // case 1-2-3: non of them have enough children so merge 2 children
            else {
                BTNode* lchild = child[key_counter];
                BTNode* rchild = child[key_counter + 1];
                // put the key in rchild
                lchild->key[(t / 2) - 1] = key[key_counter];
                // move right child key and children to left child 
                for (long long int i = 0; i < rchild->num_keys; i++) {
                    lchild->key[i + (t / 2)] = rchild->key[i];
                    // if it has any children pass them to left child
                    if (lchild->is_leaf == false) {
                        lchild->child[i + (t / 2)] = rchild->child[i];
                    }
                    if (lchild->is_leaf == false && i == rchild->num_keys - 1) {
                        lchild->child[i + 1 + (t / 2)] = rchild->child[i + 1];
                    }
                }
                // move key 1 step and children 2 step backward
                for (long long int i = key_counter + 1; i < num_keys; i++) {
                    key[i - 1] = key[i];
                    child[i] = child[i + 1];
                }

                // number of key in left child increased 
                lchild->num_keys += rchild->num_keys + 1;
                // number of key in this node decreased
                key[num_keys - 1] = NULL;
                num_keys--;
                // free the memory of right child after merging
                delete(rchild);
                // recursively remove the key that is in child now
                child[key_counter]->delete_node2(k2, t);
            }
        }
    }
    // case 2: k is not accured in this node
    else {
        // case 2-1: we are at leaf node and there is not any chilren so the key was not found
        if (is_leaf) {
            return;
        }
        // case 2-2: the key is in subtrees of this node
        else {
            // case 2-2-1: key is in last key child and the child has more than allowed key
            // check if it is the last child
            long long int last_child = 0;
            if (key_counter == num_keys) {
                last_child = 1;
            }
            if (child[key_counter]->num_keys < t / 2) {
                // case 2-2-1-1: borrow key from left brother
                if (key_counter != 0 && child[key_counter - 1]->num_keys >= t / 2) {
                    BTNode* rrchild = child[key_counter];
                    BTNode* llchild = child[key_counter - 1];
                    
                    // move children and children of it forward
                    for (long long int i = rrchild->num_keys - 1; i >= 0; i--) {
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
                    // move key and children of right child backward
                    for (long long int i = 1; i < rrchild->num_keys; i++) {
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
                // case 2-2-1-3: we can't borrow from brothers so merge with next child
                else if (key_counter != num_keys) {
                    BTNode* lchild = child[key_counter];
                    BTNode* rchild = child[key_counter + 1];
                    // put the key in rchild
                    lchild->key[(t / 2) - 1] = key[key_counter];
                    // move right child key and children to left child 
                    for (long long int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                        if (lchild->is_leaf == false && i == lchild->num_keys - 1) {
                            lchild->child[i + 1 + (t / 2)] = rchild->child[i + 1];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (long long int i = key_counter + 1; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
                    }

                    // number of key in left child increased 
                    lchild->num_keys += rchild->num_keys + 1;
                    // number of key in this node decreased
                    key[num_keys - 1] = NULL;
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
                    // move right child key and children to left child 
                    for (long long int i = 0; i < lchild->num_keys; i++) {
                        lchild->key[i + (t / 2)] = rchild->key[i];
                        // if it has any children pass them to left child
                        if (lchild->is_leaf == false) {
                            lchild->child[i + (t / 2)] = rchild->child[i];
                        }
                        if (lchild->is_leaf == false && i == lchild->num_keys - 1) {
                            lchild->child[i + 1 + (t / 2)] = rchild->child[i + 1];
                        }
                    }
                    // move key 1 step and children 2 step backward
                    for (long long int i = key_counter; i < num_keys; i++) {
                        key[i - 1] = key[i];
                        child[i] = child[i + 1];
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
            // case 2-2-2: children are merged
            if (last_child && key_counter > num_keys) {
                // recursively delete
                child[key_counter - 1]->delete_node2(k, t);
            }
            // case 2-2-3: children are not merged or key_counter does not increased
            else {
                child[key_counter]->delete_node2(k, t);
            }
        }
    }
    return;
}
////////////////////////////////////////////////
Node* BTNode::predecessor(long long int key_counter) {
    BTNode* node = child[key_counter];
    // get the right most node
    while (node->is_leaf == false) {
        node = node->child[node->num_keys];
    }

    return node->key[node->num_keys-1];
    
}
/////////////////////////////////////////////////////
Node* BTNode::successor(long long int key_counter) {
    BTNode* node = child[key_counter + 1];
    // get the left most node
    while (node->is_leaf == false) {
        node = node->child[0];
    }

    return node->key[0];
    
}
///////////////////////////////////////////////////////
Node* BTNode::search_node(long long int k) {
    long long int key_counter = 0;
    // do untill you get the place of accurance of node in it or in it's subtrees
    while (key_counter < num_keys && key[key_counter]->data < k) {
        key_counter++;
    }
    // case 1: the node contains key
    if (key_counter < num_keys && key[key_counter]->data == k) {
        return key[key_counter];
    }
    // case 2: the key is not in this node and node does not have any children, so its not found
    else if (is_leaf) {
        return NULL;
    }
    // case 3: it may be in subtrees, so search recursively for it
    else {
        return child[key_counter]->search_node(k);
    }
}
///////////////////////////////////////////////////////
Node* BTNode::find_greaters(long long int k) {
    long long int key_counter = 0;
    // do untill you get the place of accurance of node in it or in it's subtrees
    while (key_counter < num_keys && key[key_counter]->data <= k) {
        key_counter++;
    }
    // case 1: the node contains key greater then k
    if (key_counter < num_keys && key[key_counter]->data > k) {
        return key[key_counter];
    }
    // case 2: the key is not in this node and node does not have any children, so its not found
    else if (is_leaf) {
        return NULL;
    }
    // case 3: it may be in subtrees, so search recursively for it
    else {
        return child[key_counter]->search_node(k);
    }
}
///////////////////////////////////////////////////////////
Node* BTNode::find_smallers(long long int k) {
    long long int key_counter = num_keys - 1;
    // do untill you get the place of accurance of node in it or in it's subtrees
    while (key_counter >= 0 && key[key_counter]->data >= k) {
        key_counter--;
    }
    // case 1: the node contains key smaller than k
    if (key_counter >= 0 && key[key_counter]->data < k) {
        return key[key_counter];
    }
    // case 2: the key is not in this node and node does not have any children, so its not found
    else if (is_leaf) {
        return NULL;
    }
    // case 3: it may be in subtrees, so search recursively for it
    else {
        return child[key_counter]->search_node(k);
    }
}
///////////////////////////////////////////////////////////
Node* BTNode::insertNonFull(long long int k) {
  int i = num_keys - 1;

  if (is_leaf == true) {
    while (i >= 0 && key[i]->data > k) {
      key[i + 1] = key[i];
      i--;
    }
    Node* node = new Node;
    node->data = k;
    node->nextField = NULL;
    node->self = this;
    key[i + 1] = node;
    num_keys = num_keys + 1;
    return node;
  }
  else {
    while (i >= 0 && key[i]->data > k)
      i--;

    if (child[i + 1]->num_keys == maxx - 1) {
      splitChild(i + 1, child[i + 1]);

      if (key[i + 1]->data < k)
        i++;
    }
    child[i + 1]->insertNonFull(k);
  }
}
/////////////////////////////////////////////////////
void BTNode::splitChild(int i, BTNode *y) {
  BTNode *z = new BTNode(y->maxx);
  z->num_keys = maxx - 1;

  for (int j = 0; j < maxx - 1; j++)
    z->key[j] = y->key[j + maxx];

  if (y->is_leaf == false) {
    for (int j = 0; j < maxx; j++)
      z->child[j] = y->child[j + maxx];
  }

  y->num_keys = maxx - 1;
  for (int j = num_keys; j >= i + 1; j--)
    child[j + 1] = child[j];

  child[i + 1] = z;

  for (int j = num_keys - 1; j >= i; j--)
    key[j + 1] = key[j];

  key[i] = y->key[maxx - 1];
  num_keys = num_keys + 1;
}
///////////////////////////////////////////////////
// Class B tree with all operations on tree
class BTree {
    private:
        BTNode* root;
        long long int t;

    public:
        BTree(long long int);
        Node* Search(long long int);
        Node* create_node(long long int, BTNode*);
        Node* Insert2(long long int);
        Node* Insert(long long int);
        void Delete(long long int);
        void Delete2(Node*);
        void Update(long long int);
        void traverse();
        Node* search_for_greaters(long long int);
        Node* search_for_smaller(long long int k);
        BTNode* get_root();

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
Node* BTree::Insert(long long int data) {
    Node* new_key = new Node;
    // case 1: the tree is empty
    if (!root) {                          
        root = new BTNode(t);                // create root node
        Node* key = create_node(data, root);
        root->key[0] = key;    // add node to root node
        root->num_keys++;
        return root->key[0];
    }
    // case 2: the root is not empty
    else {
        root = root->find_insert_place(data, root, t, root, new_key);
        return new_key;
    }
    
}
///////////////////////////////////////////////////////
Node* BTree::Insert2(long long int k) {
    Node* new_key = new Node;   
    if (root == NULL) {
        root = new BTNode(t);                // create root node
        Node* key = create_node(k, root);
        root->key[0] = key;    // add node to root node
        root->num_keys++;
        return root->key[0];
    }
    else {
        if (root->num_keys == t) {
            BTNode *s = new BTNode(t);

            s->child[0] = root;

            s->splitChild(0, root);

            int i = 0;
            if (s->key[0]->data < k)
                i++;
            new_key = s->child[i]->insertNonFull(k);

            root = s;
        }
        else
            new_key = root->insertNonFull(k);
        return new_key;
    }
}
///////////////////////////////////////////////////////
void BTree::Delete(long long int k) {
    // case 1: the tree is empty
    if (root == NULL) {
        return;
    }
    // case 2: non-empty tree
    else {
        // go deletiong from root
        root->delete_node(k, t);
    } 
}
/////////////////////////////////////////////////////
void BTree::Delete2(Node* k) {
    // case 1: the tree is empty
    if (root == NULL) {
        return;
    }
    // case 2: non-empty tree
    else {
        // go deletiong from root
        root->delete_node2(k, t);
        // root->deletion(k, t);
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
Node* BTree::search_for_greaters(long long int k) {

    return root->find_greaters(k);
}
//////////////////////////////////////////////////////
Node* BTree::search_for_smaller(long long int k) {

    return root->find_smallers(k);
}
//////////////////////////////////////////////////////
BTNode* BTree::get_root() {
    return root;
} 
//////////////////////////////////////////////////////
long long int hash_string(string s) {
    // long long int h = 0;
    // for (int i = 0; i < s.length(); i++) {
    //     h += int(s[i]) * (i + 1);
    // }
    // return h;
    char hash_table[37] = {'/','0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    
    long long int hashed = 0;
    for (int i = 0; i < s.length(); i++) {
        char ch = s[i];
        int h = 0;
        for (int j = 0; j < 37; j++) {
            if (ch == hash_table[j]) {
                h = j;
                break;
            }
        }
        hashed += h * floor(pow(37, i));
    }
    return hashed;
}
//////////////////////////////////////////////////////
string dehash_string(long long int hashed) {
    char hash_table[37] = {'/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    string dehash = "";
    while (hashed != 0) {
        int d = hashed % 37;
        dehash += hash_table[d];
        hashed /= 37;
    }
    return dehash;
}
//////////////////////////////////////////////////////
long long int hash_date(string date) {
    long long int h = 0;
    int first = 0, second = 0;
    for (int i = 0; i < date.length(); i++) {
        if (date[i] == '/' && !first) {
            first = i;
        }
        else if (date[i] == '/' && !second) {
            second = i;
        }
    }
    string year = date.substr(0, 4);
    string month = date.substr(first + 1, second - first - 1);
    string day = date.substr(second + 1);
    h += stoi(year);
    h *= 100;
    h += stoi(month);
    h *= 100;
    h += stoi(day);

    return h;
}
/////////////////////////////////////////////////////
string dehash_date(long long int d) {
    string dehash = "";
    string date = to_string(d);
    string year = date.substr(0, 4);
    string month = date.substr(4, 2);
    string day = date.substr(6, 2);
    dehash += year;
    dehash += '/';
    if (month[0] == '0') {
        dehash += month[1];
    }
    else {
        dehash += month;
    }
    dehash += '/';
    if (day[0] == '0') {
        dehash += day[1];
    }
    else {
        dehash += day;
    }

    return dehash;
}
////////////////////////////////////////////////////
bool check_string_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (int(s[i]) < 97 || int(s[i]) > 122) {
            // not small letter
            if (!(int(s[i]) >= 48 && int(s[i]) <= 57)) {
                // not number 
                return false;
            }
        }
    }
    return true;
}
////////////////////////////////////////////////////
bool check_date_validation(string d) {
    if (d.length() > 10 || d.length() < 8) {
        return false;             // not enough or more than enough characters
    }
    int slash = 0, f_slash, s_slash;
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
        return false;       // date should contaion 2 /
    }
    if (!(s_slash == 6 || s_slash == 7)) {
        return false;
    }
    // invalid characters
    for (int i = 0; i < d.length(); i++) {
        if (int(d[i]) > 57 || int(d[i]) < 47) {
            return false;
        }
    }
    string month = d.substr(5, s_slash - f_slash);
    int mon = stoi(month);
    if (mon < 1 || mon > 12) {
        return false;
    }
    string day = d.substr(s_slash + 1);
    int dd = stoi(day);
    if (dd < 1 || dd > 31) {
        return false;
    }
    return true;
}
////////////////////////////////////////////////////
bool check_int_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (int(s[i]) < 48 || int(s[i]) > 57) {
            return false;
        }
    }
    return true;
}
////////////////////////////////////////////////////
class Query {
    private:
        vector<string> tokens;
    public:
        void parse_query(string);
        vector<string> get_tokens();
};
//////////////////////////////////////////////////////////
void Query::parse_query(string input) {
    string inputed = "";
    // delete "" from strings
    for (int i = 0; i < input.length(); i++) {
        if (int(input[i]) != 34) {
            inputed += input[i];
        }
    }
    // input = inputed;
    // string inputed = "";
    // // delete " " from strings conditions
    // for (int i = 0; i < input.length(); i++) {
    //     if (int(input[i]) == ' ' && (input[i - 1] == '=' || input[i - 1] == '>' || input[i - 1] == '<')) {
    //         // inputed += input[i];

    //     }
    //     else {
    //         inputed += input[i];
    //     }
    // }
    input = inputed;
    string word = "";
 
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
}
////////////////////////////////////////////////////////
vector<string> Query::get_tokens() {
    return tokens;
}
////////////////////////////////////////////////////////
class Table {
    private:
        string table_name;
        int table_id, num_inserted;
        vector<vector<string>> fields;
        vector<BTree*> columns;
        bool* id_table;

    public:
        Table(string);
        string get_name();
        void create_filed(string, string);
        void show_fields();
        void set_id(int);
        int get_id();
        void insert(vector<string>);
        void insert_for_update(vector<string>, vector<long long int>);
        long long int find_id_to_insert();
        void show_columns_info();
        void show(BTNode*);
        void show_equal(BTNode*, long long int, int, vector<Node*> &);
        void show_greater(BTNode*, long long int, int, vector<Node*> &);
        void show_smaller(BTNode*, long long int, int, vector<Node*> &);
        void show_columns_info_ordered(Node*);
        Node* show_equals_ordered(Node*, long long int, int);
        Node* show_greaters_ordered(Node*, long long int, int);
        Node* show_smallers_ordered(Node*, long long int, int);
        void deletion(vector<string>);
        void deletion_or(vector<string>);
        void deletion_and(vector<string>);
        void deletion_for_update(vector<string>);
        void update(vector<string>);
        vector<vector<string>> select(vector<string>, int);
        void check_for_equal(BTNode*, long long int, vector<Node*>&);
        void check_for_greater(BTNode*, long long int, vector<Node*>&);
        void check_for_smaller(BTNode*, long long int, vector<Node*>&);
};
////////////////////////////////////////////////////////
Table::Table(string name) {
    table_name = name;
    num_inserted = 0;
    fields.push_back({});
    fields.push_back({});

    // create a table for ids to find the first non-used id
    id_table = new bool[1000000];
    for (long long int i = 0; i < 1000000; i++) {
        id_table[i] = true;
    }
}
////////////////////////////////////////////////////////
string Table::get_name() {
    return table_name;
}
///////////////////////////////////////////////////////
void Table::show_fields() {
    for (int i = 0; i < fields[0].size(); i++) {
        cout << i + 1 << "-" << fields[0][i] << " " << fields[1][i] << endl;
    }
}
//////////////////////////////////////////////////////
void Table::create_filed(string field_name, string field_type) {
    // supported type: string, date, int
    if (field_type == "string" || field_type == "timestamp" || field_type == "int") {
        fields[0].push_back(field_name);
        fields[1].push_back(field_type);
        columns.push_back(new BTree(5));
    }
}
///////////////////////////////////////////////////////
void Table::show_columns_info_ordered(Node* col) {
    for (int i = 0; i < columns.size(); i++) {
        cout << col->data << " ";
        col = col->nextField;
    }
    cout << endl;
}
/////////////////////////////////////////////////////
Node* Table::show_equals_ordered(Node* col, long long int h, int colu) {
    for (int i = 0; i < columns.size(); i++) {
        if (col && i == colu && col->data == h) {
            cout << col->data << endl;
            return col;
        }
        if(col) {
            col = col->nextField;
        }
            // cout << col->data << endl;
            // col = col->nextField;
    }
    return NULL;
}
/////////////////////////////////////////////////////
Node* Table::show_greaters_ordered(Node* col, long long int h, int colu) {
    for (int i = 0; i < columns.size(); i++) {
        if (i == colu && col->data > h) {
            return col;
        }
        col = col->nextField;
    }
    return NULL;
}
/////////////////////////////////////////////////////
Node* Table::show_smallers_ordered(Node* col, long long int h, int colu) {
    for (int i = 0; i < columns.size(); i++) {
        if (i == colu && col->data < h) {
            // cout <<endl<< col->data << " s " << h <<endl;
            return col;
        }
        col = col->nextField;
    }
    return NULL;
}
/////////////////////////////////////////////////////
void Table::show(BTNode* node) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            show(node->child[i]);
        show_columns_info_ordered(node->key[i]);
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        show(node->child[i]);
    }
}
////////////////////////////////////////////////////
void Table::show_equal(BTNode* node, long long int h, int colu, vector<Node*> &ans) {
    long long int i;
    
    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            show_equal(node->child[i], h, colu, ans);
        Node* node1 = show_equals_ordered(node->key[i], h, colu);
        if (node1) {
            ans.push_back(node1);
        }
        // show_columns_info_ordered(node->key[i]);
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        show_equal(node->child[i], h, colu, ans);
    }
}
////////////////////////////////////////////////////
void Table::show_greater(BTNode* node, long long int h, int colu, vector<Node*> &ans) {
    long long int i;
    
    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            show_greater(node->child[i], h, colu, ans);
        Node* node1 = show_greaters_ordered(node->key[i], h, colu);
        if (node1) {
            ans.push_back(node1);
        }
        // show_columns_info_ordered(node->key[i]);
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        show_greater(node->child[i], h, colu, ans);
    }
}
////////////////////////////////////////////////////
void Table::show_smaller(BTNode* node, long long int h, int colu, vector<Node*> &ans) {
    long long int i;
    
    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            show_smaller(node->child[i], h, colu, ans);
        Node* node1 = show_smallers_ordered(node->key[i], h, colu);
        if (node1) {
            ans.push_back(node1);
        }
        // show_columns_info_ordered(node->key[i]);
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        show_smaller(node->child[i], h, colu, ans);
    }
}
////////////////////////////////////////////////////
void Table::set_id(int id) {
    table_id = id;
}
///////////////////////////////////////////////////////
int Table::get_id() {
    return table_id;
}
//////////////////////////////////////////////////////
void Table::insert(vector<string> tokens) {
    int num_fields = tokens.size() - 4;
    // number of fileds is not correct
    if (num_fields != columns.size() - 1) {
        return;
    }
    else {
        vector<Node*> inserted_list;
        long long int id = find_id_to_insert();
        Node* insertt = columns[0]->Insert(id);
        // Node* insertt = columns[0]->Insert2(id);
        id_table[id] = false;
        // inserted_list.push_back(columns[0]->Search(id));
        inserted_list.push_back(insertt);
        long long int h;
        
        for (int i = 4; i < tokens.size(); i++) {
            string type_of = fields[1][i - 3];
            // convert to right type
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
                    cerr << e << '\n';
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
                    cerr << e << '\n';
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
                    cerr << e << '\n';
                }
            }
            insertt = columns[i - 3]->Insert(h);
            // insertt = columns[i - 3]->Insert2(h);
            // cout << insertt->data << "--";
            // inserted_list.push_back(columns[i - 3]->Search(h));
            inserted_list.push_back(insertt);
        }

        // fix next_fields of nodes
        for (int i = 0; i < inserted_list.size() - 1; i++) {
            // cout << "oo";
            inserted_list[i]->nextField = inserted_list[i + 1];
        }
        inserted_list[inserted_list.size() - 1]->nextField = inserted_list[0];
        num_inserted++;
        // cout << "pP";

    }
    // show(columns[0]->get_root());
    
}
/////////////////////////////////////////////////////
void Table::insert_for_update(vector<string> tokens, vector<long long int> ids) {
    int num_fields = tokens.size() - 5;
    // number of fileds is not correct
    if (num_fields != columns.size() - 1) {
        return;
    }
    else {
        for (int k = 0; k < ids.size(); k++) {
            vector<Node*> inserted_list;
            long long int id = ids[k];
            Node* insertt = columns[0]->Insert(id);
            // Node* insertt = columns[0]->Insert2(id);
            // cout << insertt->data ;
            // id_table[id] = false;
            // inserted_list.push_back(columns[0]->Search(id));
            inserted_list.push_back(insertt);
            long long int h;
            
            for (int i = 3; i < columns.size() + 2; i++) {
                string type_of = fields[1][i - 2];
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
                        cerr << e << '\n';
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
                        cerr << e << '\n';
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
                        cerr << e << '\n';
                    }
                }
                // cout << h;
                // columns[i - 2]->traverse();
                insertt = columns[i - 2]->Insert(h);
                // insertt = columns[i - 2]->Insert2(h);
                // cout << insertt->data << " *** ";
                // inserted_list.push_back(columns[i - 3]->Search(h));
                inserted_list.push_back(insertt);
            }

            // fix next_fields of nodes
            for (int i = 0; i < inserted_list.size() - 1; i++) {
                inserted_list[i]->nextField = inserted_list[i + 1];
            }
            inserted_list[inserted_list.size() - 1]->nextField = inserted_list[0];
            // Node* node = columns[0]->Search(inserted_list[0]->data);
            // for (int i = 0; i < inserted_list.size(); i++) {
            //     node = node->nextField;
            // }
        }
        
    }
    
}
/////////////////////////////////////////////////////
long long int Table::find_id_to_insert() {
    long long int id;
    for (long long int i = 1; i < 1000000; i++) {
        if (id_table[i]) {
            id = i;
            break;
        }
    }
    return id;
}
//////////////////////////////////////////////////////
void Table::show_columns_info() {
    for (int i = 0; i < columns.size(); i++) {
        cout << fields[0][i] << endl;
        columns[i]->traverse();
        cout << endl<<"----------------"<<endl;
    }
}
//////////////////////////////////////////////////////
void Table::deletion(vector<string> tokens) {
    string condition = tokens[4], fields_name, operation, state;
    int pos = -1;
    pos = condition.find("==");
    if (pos != -1) {     // condition is ==
        fields_name = condition.substr(0, pos);
        operation = condition.substr(pos, 2);
        state = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {      // condition is >
            fields_name = condition.substr(0, pos);
            operation = condition.substr(pos, 1);
            state = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");       // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation = condition.substr(pos, 1);
                state = condition.substr(pos + 1);
            }
            else {                // condition is undefined, so break up
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
            h = hash_string(state);
        }
        else if (fields[1][col] == "timestamp") {
            h = hash_date(state);
        }
        else if (fields[1][col] == "int") {
            h = stoll(state);
        }
        // do conditions
        if (operation == "==") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            check_for_equal(rt, h, all_equal_nodes);       // find all nodes that are equal to statement
            // show_equal(bt->get_root(), h, col % columns.size(), all_equal_nodes);
            // cout << all_equal_nodes.size()<<endl;
            for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_equal_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col++;
                }
            }
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            // columns[3]->traverse();
            check_for_greater(rt, h, all_greater_nodes);       // find all nodes that are greater than statement
            // show_greater(bt->get_root(), h, col % columns.size(), all_greater_nodes);

            for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true;      // free the deleted nodes id
                    }

                    bt->Delete2(all_greater_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col++;
                }
            }
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            check_for_smaller(rt, h, all_smaller_nodes);       // find all nodes that are smaller than statement
            // show_smaller(bt->get_root(), h, col % columns.size(), all_smaller_nodes);

            for (int i = 0; i < all_smaller_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_smaller_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col++;
                }
            }
        }
        num_inserted--;
        // show(columns[0]->get_root());
    }
    // column not found
    else {
        return;
    }

}
//////////////////////////////////////////////////////
void Table::deletion_or(vector<string> tokens) {
    string condition1 = tokens[4], condition2 = tokens[6], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    pos1 = condition1.find("==");
    if (pos1 != -1) {     // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1 = condition1.substr(pos1, 2);
        state1 = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {      // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1 = condition1.substr(pos1, 1);
            state1 = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");       // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1 = condition1.substr(pos1, 1);
                state1 = condition1.substr(pos1 + 1);
            }
            else {                // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {     // condition is ==
        fields_name2 = condition1.substr(0, pos2);
        operation2 = condition2.substr(pos2, 2);
        state2 = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {      // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2 = condition2.substr(pos2, 1);
            state2 = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");       // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2 = condition2.substr(pos2, 1);
                state2 = condition2.substr(pos2 + 1);
            }
            else {                // condition is undefined, so break up
                return;
            }
        }
    }
    // find the column that we want to get query on it
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
            if (check_string_validation(state1))
                h = hash_string(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
                
        }
        else if (fields[1][col1] == "timestamp") {
            if (check_date_validation(state1))
                h = hash_date(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        else if (fields[1][col1] == "int") {
            if (check_int_validation(state1))
                h = stoll(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        // do conditions
        if (operation1 == "==") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            check_for_equal(rt, h, all_equal_nodes);       // find all nodes that are equal to statement
            // show_equal(bt->get_root(), h, col % columns.size(), all_equal_nodes);
            // cout << all_equal_nodes.size()<<endl;
            for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_equal_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == ">") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            // columns[3]->traverse();
            check_for_greater(rt, h, all_greater_nodes);       // find all nodes that are greater than statement
            // show_greater(bt->get_root(), h, col % columns.size(), all_greater_nodes);

            for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true;      // free the deleted nodes id
                    }

                    bt->Delete2(all_greater_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        else if (operation1 == "<") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            check_for_smaller(rt, h, all_smaller_nodes);       // find all nodes that are smaller than statement
            // show_smaller(bt->get_root(), h, col % columns.size(), all_smaller_nodes);

            for (int i = 0; i < all_smaller_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col1 % columns.size()];
                    if (fields[0][col1 % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_smaller_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col1++;
                }
            }
        }
        // show(columns[0]->get_root());
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
            if (check_string_validation(state2))
                h = hash_string(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
                
        }
        else if (fields[1][col2] == "timestamp") {
            if (check_date_validation(state2))
                h = hash_date(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        else if (fields[1][col2] == "int") {
            if (check_int_validation(state2))
                h = stoll(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        // do conditions
        if (operation2 == "==") {
            BTree* bt = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            check_for_equal(rt, h, all_equal_nodes);       // find all nodes that are equal to statement
            // show_equal(bt->get_root(), h, col % columns.size(), all_equal_nodes);
            // cout << all_equal_nodes.size()<<endl;
            for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_equal_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == ">") {
            BTree* bt = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            // columns[3]->traverse();
            check_for_greater(rt, h, all_greater_nodes);       // find all nodes that are greater than statement
            // show_greater(bt->get_root(), h, col % columns.size(), all_greater_nodes);

            for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true;      // free the deleted nodes id
                    }

                    bt->Delete2(all_greater_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        else if (operation2 == "<") {
            BTree* bt = columns[col2 % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            check_for_smaller(rt, h, all_smaller_nodes);       // find all nodes that are smaller than statement
            // show_smaller(bt->get_root(), h, col % columns.size(), all_smaller_nodes);

            for (int i = 0; i < all_smaller_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col2 % columns.size()];
                    if (fields[0][col2 % columns.size()] == "id") {
                        id_table[all_smaller_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_smaller_nodes[i]);   // delete the node
                    // bt->traverse();
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col2++;
                }
            }
        }
        // show(columns[0]->get_root());
    }
    // column not found
    else {
        return;
    }

}
//////////////////////////////////////////////////////
void Table::deletion_and(vector<string> tokens) {
    string condition1 = tokens[4], condition2 = tokens[6], fields_name1, operation1, state1,
    fields_name2, operation2, state2;
    int pos1 = -1, pos2 = -1;
    // cout << state1 << " -- "<< state2;
    pos1 = condition1.find("==");
    if (pos1 != -1) {     // condition is ==
        fields_name1 = condition1.substr(0, pos1);
        operation1 = condition1.substr(pos1, 2);
        state1 = condition1.substr(pos1 + 2);
    }
    else {
        pos1 = condition1.find(">");
        if (pos1 != -1) {      // condition is >
            fields_name1 = condition1.substr(0, pos1);
            operation1 = condition1.substr(pos1, 1);
            state1 = condition1.substr(pos1 + 1);
        }
        else {
            pos1 = condition1.find("<");       // condition is <
            if (pos1 != -1) {
                fields_name1 = condition1.substr(0, pos1);
                operation1 = condition1.substr(pos1, 1);
                state1 = condition1.substr(pos1 + 1);
            }
            else {                // condition is undefined, so break up
                return;
            }
        }
    }
    pos2 = condition2.find("==");
    if (pos2 != -1) {     // condition is ==
        fields_name2 = condition2.substr(0, pos2);
        operation2 = condition2.substr(pos2, 2);
        state2 = condition2.substr(pos2 + 2);
    }
    else {
        pos2 = condition2.find(">");
        if (pos2 != -1) {      // condition is >
            fields_name2 = condition2.substr(0, pos2);
            operation2 = condition2.substr(pos2, 1);
            state2 = condition2.substr(pos2 + 1);
        }
        else {
            pos2 = condition2.find("<");       // condition is <
            if (pos2 != -1) {
                fields_name2 = condition2.substr(0, pos2);
                operation2 = condition2.substr(pos2, 1);
                state2 = condition2.substr(pos2 + 1);
            }
            else {                // condition is undefined, so break up
                return;
            }
        }
    }
    // cout << state1 << " -- " << state2 << endl;
    // find the column that we want to get query on it
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
    // cout << col1 << " _ "<< fields_name2 << endl;
    // column found
    if (col1 != -1 && col2 != -1) {
        // cout << col1 << " " << col2;
        long long int h;
        vector<Node*> all_nodes1, all_nodes2;
        // hash the statement base on type of column
        if (fields[1][col1] == "string") {
            if (check_string_validation(state1))
                h = hash_string(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
                
        }
        else if (fields[1][col1] == "timestamp") {
            if (check_date_validation(state1))
                h = hash_date(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        else if (fields[1][col1] == "int") {
            if (check_int_validation(state1))
                h = stoll(state1);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        // do conditions
        if (operation1 == "==") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            // vector<Node*> all_nodes;
            check_for_equal(rt, h, all_nodes1);       // find all nodes that are equal to statement
            // show_equal(bt->get_root(), h, col % columns.size(), all_equal_nodes);
            // cout << all_equal_nodes.size()<<endl;
            // for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
            //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
            //         BTree* bt = columns[col1 % columns.size()];
            //         if (fields[0][col1 % columns.size()] == "id") {
            //             id_table[all_equal_nodes[i]->data] = true;      // free the deleted nodes id
            //         }
            //         bt->Delete2(all_equal_nodes[i]);   // delete the node
            //         // bt->traverse();
            //         all_equal_nodes[i] = all_equal_nodes[i]->nextField;
            //         col1++;
            //     }
            // }
        }
        else if (operation1 == ">") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            // columns[3]->traverse();
            check_for_greater(rt, h, all_nodes1);       // find all nodes that are greater than statement
            // show_greater(bt->get_root(), h, col % columns.size(), all_greater_nodes);

            // for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
            //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
            //         BTree* bt = columns[col1 % columns.size()];
            //         if (fields[0][col1 % columns.size()] == "id") {
            //             id_table[all_greater_nodes[i]->data] = true;      // free the deleted nodes id
            //         }

            //         bt->Delete2(all_greater_nodes[i]);   // delete the node
            //         // bt->traverse();
            //         all_greater_nodes[i] = all_greater_nodes[i]->nextField;
            //         col1++;
            //     }
            // }
        }
        else if (operation1 == "<") {
            BTree* bt = columns[col1 % columns.size()];
            BTNode* rt = bt->get_root();
            // vector<Node*> all_smaller_nodes;
            check_for_smaller(rt, h, all_nodes1);       // find all nodes that are smaller than statement
            // cout << "got";
            // show_smaller(bt->get_root(), h, col % columns.size(), all_smaller_nodes);

            // for (int i = 0; i < all_smaller_nodes.size(); i++) {   // for all nodes that had the condition
            //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
            //         BTree* bt = columns[col1 % columns.size()];
            //         if (fields[0][col1 % columns.size()] == "id") {
            //             id_table[all_smaller_nodes[i]->data] = true;      // free the deleted nodes id
            //         }
            //         bt->Delete2(all_smaller_nodes[i]);   // delete the node
            //         // bt->traverse();
            //         all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
            //         col1++;
            //     }
            // }
        }
        if (fields[1][col2] == "string") {
            if (check_string_validation(state2))
                h = hash_string(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
                
        }
        else if (fields[1][col2] == "timestamp") {
            if (check_date_validation(state2))
                h = hash_date(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        else if (fields[1][col2] == "int") {
            if (check_int_validation(state2))
                h = stoll(state2);
            else {
                cout << "Invalid command" << endl;
                return;
            }
        }
        // cout << "sixe: " << all_nodes1.size() << endl;
        for (int i = 0; i < all_nodes1.size(); i++) {
            Node* check = all_nodes1[i];
            // cout << check->data << " - ";
            int move = col1, counter = 0;
            while ((move + counter) % columns.size() != col2) {
                // cout << check->data << " + ";
                check = check->nextField;
                counter++;
            }
            // cout << condition2 << endl;
            if (operation2 == "==") {
                // cout << check->data << " = " << h << endl;
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
        for (int i = 0; i < all_nodes2.size(); i++) {   // for all nodes that had the condition
            for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                BTree* bt = columns[col1 % columns.size()];
                if (fields[0][col1 % columns.size()] == "id") {
                    id_table[all_nodes2[i]->data] = true;      // free the deleted nodes id
                }
                bt->Delete2(all_nodes2[i]);   // delete the node
                // bt->traverse();
                all_nodes2[i] = all_nodes2[i]->nextField;
                col1++;
            }
        }
        // show(columns[0]->get_root());
    }
    // column not found
    else {
        return;
    }
}
//////////////////////////////////////////////////////
void Table::deletion_for_update(vector<string> tokens) {
    string condition = tokens[columns.size() + 3], fields_name, operation, state;
    int pos = -1;
    pos = condition.find("==");
    if (pos != -1) {     // condition is ==
        fields_name = condition.substr(0, pos);
        operation = condition.substr(pos, 2);
        state = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {      // condition is >
            fields_name = condition.substr(0, pos);
            operation = condition.substr(pos, 1);
            state = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");       // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation = condition.substr(pos, 1);
                state = condition.substr(pos + 1);
            }
            else {                // condition is undefined, so break up
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
            h = hash_string(state);
        }
        else if (fields[1][col] == "timestamp") {
            h = hash_date(state);
        }
        else if (fields[1][col] == "int") {
            h = stoll(state);
        }
        vector<Node*> all_nodes;
        // do conditions
        if (operation == "==") {
            // cout << 'y';
            // for (int i = 0; i < 1000000; i++) {
            //     BTree* bt = columns[0];
            //     bt->traverse();
            //     if (!id_table[i]) {
            //         // cout << i;
            //         Node* idd = bt->Search(id_table[i]);
            //         cout << '/'<<idd->data;
            //         for (int j = 0; j < columns.size(); j++) {
            //             if (j == col) {
            //                 cout << j;
            //                 if (idd->data == h) {
            //                     all_nodes.push_back(idd);
            //                 }
            //             }
            //             idd = idd->nextField;
            //         }
            //     }
            // }
            // for (int i = 0; i < all_nodes.size(); i++) {
            //     cout << all_nodes[i]->data << " = ";
            // }
            // cout <<  endl;
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_equal_nodes;
            check_for_equal(rt, h, all_equal_nodes);       // find all nodes that are equal to statement
            // show_equal(rt, h, col, all_equal_nodes);
            // cout << "got";
            // for (int i = 0; i < all_equal_nodes.size(); i++) {
            //     // cout<<all_equal_nodes[i]->nextField->data << " * "<<endl;
            // }
            for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_equal_nodes[i]->data);      // save the deleted nodes id
                    }
                    bt->Delete2(all_equal_nodes[i]);   // delete the node
                    // bt->Delete(all_equal_nodes[i]->data);   // delete the node
                    // bt->traverse();
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col++;
                }
            }
            // cout << "dd";
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            // columns[3]->traverse();
            check_for_greater(rt, h, all_greater_nodes);       // find all nodes that are greater than statement
            
            for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_greater_nodes[i]->data);      // save the deleted nodes id
                    }

                    bt->Delete2(all_greater_nodes[i]);   // delete the node
                    // bt->Delete(all_greater_nodes[i]->data);   // delete the node
                    // bt->traverse();
                    all_greater_nodes[i] = all_greater_nodes[i]->nextField;
                    col++;
                }
            }
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_smaller_nodes;
            check_for_smaller(rt, h, all_smaller_nodes);       // find all nodes that are smaller than statement
            
            for (int i = 0; i < all_smaller_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_smaller_nodes[i]->data);      // save the deleted nodes id
                    }
                    bt->Delete2(all_smaller_nodes[i]);   // delete the node
                    // bt->Delete(all_smaller_nodes[i]->data);   // delete the node
                    // bt->traverse();
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col++;
                }
            }
        }
        // columns[3]->traverse();
        insert_for_update(tokens, ids);
    }
    // column not found
    else {
        return;
    }

}
//////////////////////////////////////////////////////
void Table::update(vector<string> tokens) {
    deletion_for_update(tokens);
    // return;
    // cout << "done";
    // int pos_tok =  columns.size() + 3;
    // string condition = tokens[pos_tok], fields_name, operation, state;
    // int pos = -1;
    // // find the condition phrase
    // pos = condition.find("==");
    // if (pos != -1) {     // condition is ==
    //     fields_name = condition.substr(0, pos);
    //     operation = condition.substr(pos, 2);
    //     state = condition.substr(pos + 2);
    // }
    // else {
    //     pos = condition.find(">");
    //     if (pos != -1) {      // condition is >
    //         fields_name = condition.substr(0, pos);
    //         operation = condition.substr(pos, 1);
    //         state = condition.substr(pos + 1);
    //     }
    //     else {
    //         pos = condition.find("<");       // condition is <
    //         if (pos != -1) {
    //             fields_name = condition.substr(0, pos);
    //             operation = condition.substr(pos, 1);
    //             state = condition.substr(pos + 1);
    //         }
    //         else {                // condition is undefined, so break up
    //             return;
    //         }
    //     }
    // }
    // // find the column that we want to get query on it
    // int col = -1;
    // for (int i = 0; i < fields[0].size(); i++) {
    //     if (fields[0][i] == fields_name) {
    //         col = i;
    //         break;
    //     }
    // }
    // // column found
    // if (col != -1) {
    //     long long int h, h2;
    //     // hash the statement base on type of column
    //     if (fields[1][col] == "string") {
    //         h = hash_string(state);
    //     }
    //     else if (fields[1][col] == "timestamp") {
    //         h = hash_date(state);
    //     }
    //     else if (fields[1][col] == "int") {
    //         h = stoll(state);
    //     }
    //     // what to change
    //     vector<string> change;
    //     vector<long long int> hash_change;
    //     vector<int> ids;
    //     vector<Node*> ids_node;
    //     for (int i = 3; i < columns.size() + 2; i++) {
    //         change.push_back(tokens[i]);
    //     }
    //     for (int i = 0; i < change.size(); i++) {
    //         if (fields[1][i + 1] == "string") {
    //             h2 = hash_string(change[i]);
    //         }
    //         else if (fields[1][i + 1] == "timestamp") {
    //             h2 = hash_date(change[i]);
    //         }
    //         else if (fields[1][i + 1] == "int") {
    //             h2 = stoll(change[i]);
    //         }
    //         hash_change.push_back(h2);
    //     }
    //     vector<Node*> all_nodes;
    //     // do conditions
    //     if (operation == "==") {
    //         BTree* bt = columns[col % columns.size()];
    //         BTNode* rt = bt->get_root();
    //         check_for_equal(rt, h, all_nodes);       // find all nodes that are equal to statement
    //         for (int i = 0; i < all_nodes.size(); i++) {
    //             Node* x = all_nodes[i];
    //             for (int j = 0; j < columns.size(); j++) {
    //                 if (fields[0][col % columns.size()] != "id") {
    //                     x->data = hash_change[col % columns.size() - 1];
    //                 }
    //                 // cout << x->data <<endl;
    //                 x = x->nextField;
    //                 col++;
    //             }
    //         }
            
    //         // for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
    //         //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
    //         //         BTree* bt = columns[col % columns.size()];
    //         //         if (fields[0][col % columns.size()] == "id") {
    //         //             Node* id_node = columns[col % columns.size()]->Search(all_nodes[i]->data);
    //         //             ids_node.push_back(id_node);
    //         //             // Node* move = id_node;
    //         //             // for (int k = 0; k < columns.size(); k++) {
    //         //             //     move->nextField = NULL;
    //         //             //     Node* next = move->nextField;
    //         //             //     move = move->nextField;
                            
    //         //             // }
    //         //             ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
    //         //         }
    //         //         else {
    //         //             columns[col % columns.size()]->Delete2(all_nodes[i]);   // delete the node
    //         //             columns[col % columns.size()]->traverse();
    //         //         }
                    
    //         //         all_nodes[i] = all_nodes[i]->nextField;
    //         //         col++;
    //         //     }
    //         // }
            
    //     }
    //     else if (operation == ">") {
    //         BTree* bt = columns[col % columns.size()];
    //         BTNode* rt = bt->get_root();
    //         check_for_greater(rt, h, all_nodes);       // find all nodes that are equal to statement
    //         for (int i = 0; i < all_nodes.size(); i++) {
    //             Node* x = all_nodes[i];
    //             for (int j = 0; j < columns.size(); j++) {
    //                 if (fields[0][col % columns.size()] != "id") {
    //                     x->data = hash_change[col % columns.size() - 1];
    //                 }
    //                 cout << x->data <<endl;
    //                 x = x->nextField;
    //                 col++;
    //             }
    //         }
    //         // BTree* bt = columns[col % columns.size()];
    //         // BTNode* rt = bt->get_root();
    //         // check_for_greater(rt, h, all_nodes);       // find all nodes that are equal to statement
    //         // for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
    //         //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
    //         //         BTree* bt = columns[col % columns.size()];
    //         //         if (fields[0][col % columns.size()] == "id") {

    //         //             ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
    //         //         }

    //         //         bt->Delete2(all_nodes[i]);   // delete the node
    //         //         // bt->traverse();
    //         //         all_nodes[i] = all_nodes[i]->nextField;
    //         //         col++;
    //         //     }
    //         // }
    //     }
    //     else if (operation == "<") {
    //         BTree* bt = columns[col % columns.size()];
    //         BTNode* rt = bt->get_root();
    //         check_for_smaller(rt, h, all_nodes);       // find all nodes that are equal to statement
    //         for (int i = 0; i < all_nodes.size(); i++) {
    //             Node* x = all_nodes[i];
    //             for (int j = 0; j < columns.size(); j++) {
    //                 if (fields[0][col % columns.size()] != "id") {
    //                     x->data = hash_change[col % columns.size() - 1];
    //                 }
    //                 cout << x->data <<endl;
    //                 x = x->nextField;
    //                 col++;
    //             }
    //         }
    //         // BTree* bt = columns[col % columns.size()];
    //         // BTNode* rt = bt->get_root();
    //         // check_for_smaller(rt, h, all_nodes);       // find all nodes that are equal to statement
            
    //         // for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
    //         //     for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
    //         //         BTree* bt = columns[col % columns.size()];
    //         //         if (fields[0][col % columns.size()] == "id") {
    //         //             ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
    //         //         }

    //         //         bt->Delete2(all_nodes[i]);   // delete the node
    //         //         // bt->traverse();
    //         //         all_nodes[i] = all_nodes[i]->nextField;
    //         //         col++;
    //         //     }
    //         // }
            
    //     }
    // }
    // //     for (int i = 0; i < ids_node.size(); i++) {
    // //         Node* id_node = ids_node[i];
    // //         for (int j = 0; j < hash_change.size(); j++) {
    // //             // BTree* bt = columns[j + 1];
    // //             Node* inserted = columns[j + 1]->Insert(hash_change[j]);
    // //             columns[j + 1]->traverse();
    // //             id_node->nextField = NULL;
    // //             id_node->nextField = inserted;
    // //             id_node = id_node->nextField;
    // //             columns[j + 1]->traverse();
    // //         }
    // //     }
    // //     // for (int i = 0; i < all_nodes.size(); i++) {
    // //     // }
    // //     // for (int i = 0; i < ids_node.size(); i++) {
    // //     // }
    // //     // insert updated datas to tree
    // //     // for (int i = 0; i < all_nodes.size(); i++) {    // for the exact number of deleted nodes
    // //         // vector<Node*> inserted_list;
    // //         // long long int id = ids[i];
    // //         // Node* insertt = columns[0]->Insert(id);
    // //         // inserted_list.push_back(insertt);
    // //         // for (int j = 0; j < hash_change.size(); j++) {
    // //         //     insertt = columns[j + 1]->Insert(hash_change[j]);
    // //         //     inserted_list.push_back(insertt);
    // //         // }
    // //         // // fix next_fields of nodes
    // //         // for (int j = 0; j < columns.size() - 1; j++) {
    // //         //     inserted_list[j]->nextField = inserted_list[j + 1];
    // //         // }
    // //         // inserted_list[columns.size() - 1]->nextField = inserted_list[0];
    // //     // }
    // //     // return;
    // // }
    // // // column not found
    // // else {
       
    // //     return;
    // // }
}
/////////////////////////////////////////////////////
void Table::check_for_equal(BTNode* node, long long int h, vector<Node*> & change) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_equal(node->child[i], h, change);
        if (node->key[i]->data == h) {
            change.push_back(node->key[i]);
        }
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        check_for_equal(node->child[i], h, change);
    }
}
//////////////////////////////////////////////////////////////////////////////
void Table::check_for_greater(BTNode* node, long long int h, vector<Node*> & change) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_greater(node->child[i], h, change);
        if (node->key[i]->data > h) {
            change.push_back(node->key[i]);
        }
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        check_for_greater(node->child[i], h, change);
    }
}
//////////////////////////////////////////////////////////////////////////////
void Table::check_for_smaller(BTNode* node, long long int h, vector<Node*> & change) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_smaller(node->child[i], h, change);
        if (node->key[i]->data < h) {
            change.push_back(node->key[i]);
        }
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        check_for_smaller(node->child[i], h, change);
    }
}
//////////////////////////////////////////////////////////////////////////////
vector<vector<string>> Table::select(vector<string> tokens, int table_ind) {
    vector<int> selected_columns;     // columns we want their entries
    int num_sel = table_ind - 2;
    if (tokens[1] == "*") {
        num_sel = columns.size();
        for (int i = 0; i < columns.size(); i++) {
            selected_columns.push_back(i);
        }
    }
    else {
        for (int i = 0; i < num_sel; i++) {
            for (int j = 0; j < columns.size(); j++) {
                if (fields[0][j] == tokens[i + 1]) {
                    selected_columns.push_back(j);
                    break;
                }
            }
        }
    }
    int pos_tok =  table_ind + 2;
    string condition = tokens[pos_tok], fields_name, operation, state;
    int pos = -1;
    // find the condition phrase
    pos = condition.find("==");
    if (pos != -1) {     // condition is ==
        fields_name = condition.substr(0, pos);
        operation = condition.substr(pos, 2);
        state = condition.substr(pos + 2);
    }
    else {
        pos = condition.find(">");
        if (pos != -1) {      // condition is >
            fields_name = condition.substr(0, pos);
            operation = condition.substr(pos, 1);
            state = condition.substr(pos + 1);
        }
        else {
            pos = condition.find("<");       // condition is <
            if (pos != -1) {
                fields_name = condition.substr(0, pos);
                operation = condition.substr(pos, 1);
                state = condition.substr(pos + 1);
            }
            else {                // condition is undefined, so break up
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
            h = hash_string(state);
        }
        else if (fields[1][col] == "timestamp") {
            h = hash_date(state);
        }
        else if (fields[1][col] == "int") {
            h = stoll(state);
        }

        vector<Node*> all_nodes;
        // do conditions
        if (operation == "==") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_equal(rt, h, all_nodes);       // find all nodes that are equal to statement
            // show_equal(columns[0]->get_root(), h, col, all_nodes);
        }
        else if (operation == ">") {
            // columns[3]->traverse();
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_greater(rt, h, all_nodes);       // find all nodes that are equal to statement
            // show_greater(columns[0]->get_root(), h, col, all_nodes);
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_smaller(rt, h, all_nodes);       // find all nodes that are equal to statement
            // show_smaller(columns[0]->get_root(), h, col, all_nodes);
        }
        
        vector<vector<string>> answers;
        // vector<Node*> sorted1, sorted2;
        // sorted1 = all_nodes;
        vector<Node*> sorted1, sorted2;
        for (int i = 0; i < all_nodes.size(); i++) {
            Node* move = all_nodes[i];
            int cur_col = col;
            
            while (cur_col % columns.size() != 0) {
                move = move->nextField;
                cur_col++;
            }
            sorted1.push_back(move);
        }
        for (int i = 0; i < sorted1.size(); i++) {
            for (int j = i + 1; j < sorted1.size(); j++) {
                if (sorted1[i]->data > sorted1[j]->data) {
                    Node* temp = sorted1[i];
                    sorted1[i] = sorted1[j];
                    sorted1[j] = temp;
                }
            }
        }

        for (int i = 0; i < sorted1.size(); i++) {
            answers.push_back({});
            Node* move = sorted1[i];

            for (int j = 0; j < columns.size(); j++) {
                for (int k = 0; k < selected_columns.size(); k++) {
                    if (selected_columns[k] == j) {
                        if (fields[1][j] == "string") {
                            string dehash = dehash_string(move->data);
                            answers[i].push_back(dehash);
                            break;
                        }
                        else if (fields[1][j] == "timestamp") {
                            string dehash = dehash_date(move->data);
                            answers[i].push_back(dehash);
                            break;
                        }
                        else {
                            answers[i].push_back(to_string(move->data));
                            break;
                        }
                        
                    }
                }
                move = move->nextField;
            }

        }

        return answers;
    }
}
//////////////////////////////////////////////////////////////////////////////
int main() {
    
    string input;
    vector<vector<string>> all_select_queries;
    int n;
    cin >> n;
    cin.ignore();
    vector <Table*> all_tables;          // save all tables in it
    for (int q = 0; q < n; q++) {
        getline(cin, input);
        Query query;
        query.parse_query(input);
        vector<string> tokens = query.get_tokens();

        // turn tokens to sql code
        string operation = tokens[0];
        // first token shows operation to call the right method
        if (operation == "CREATE") {
            string table = tokens[1];    // second token should be TABLE
            if (table == "TABLE") {
                string table_name = tokens[2];         // third token should be name of table we wanna create
                all_tables.push_back(new Table(table_name));   // add object of table to all table objects list
                Table* table_obj = all_tables[all_tables.size() - 1];
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
            string into = tokens[1];    // second token should be INTO
            if (into == "INTO") {
                string table_name = tokens[2];         // third token should be name of table we wanna insert to
                // find the table object
                Table* table_obj = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                }
                // table found
                else {
                    table_obj->insert(tokens);
                }
            }
        }
        else if (operation == "DELETE") {
            string from = tokens[1];    // second token should be FROM
            if (from == "FROM") {
                string table_name = tokens[2];         // third token should be name of table we wanna delete from
                // find the table object
                Table* table_obj = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                }
                // table found
                else {
                    // delete with single condition
                    if (tokens.size() == 5) {
                        table_obj->deletion(tokens);
                    }
                    else if (tokens.size() == 7) {
                        if (tokens[5] == "|") 
                            table_obj->deletion_or(tokens);
                        else if (tokens[5] == "&")
                            table_obj->deletion_and(tokens);
                        // table_obj->show_columns_info();
                    }
                }

            }
            
        }
        else if (operation == "UPDATE") {
            string table_name = tokens[1];         // second token should be name of table we wanna update 
            // find the table object
            Table* table_obj = NULL;
            for (int i = 0; i < all_tables.size(); i++) {
                if (all_tables[i]->get_name() == table_name) {
                    table_obj = all_tables[i];
                    break;
                }
            }
            // the table not exists
            if (!table_obj) {
            }
            // table found
            else {
                table_obj->update(tokens);
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
                string table_name = tokens[table_ind];         // get name of table we wanna select from
                // find the table object
                Table* table_obj = NULL;
                for (int i = 0; i < all_tables.size(); i++) {
                    if (all_tables[i]->get_name() == table_name) {
                        table_obj = all_tables[i];
                        break;
                    }
                }
                // the table not exists
                if (!table_obj) {
                }
                // table found
                else {
                    vector<vector<string>> all_selects = table_obj->select(tokens, table_ind);
                    for (int k = 0; k < all_selects.size(); k++) {
                        all_select_queries.push_back(all_selects[k]);
                    }
                }
            }
        }
    }
    for (int i = 0; i < all_select_queries.size(); i++) {
        for (int j = 0; j < all_select_queries[i].size(); j++) {
            cout << all_select_queries[i][j] << " ";
        }
        cout << endl;
    }


    return 0;
}