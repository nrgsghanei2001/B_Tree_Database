// Inserting a key on a B-tree in child++

#include <iostream>
using namespace std;

class Node {
  int *key;
  int t;
  Node **child;
  int num_keys;
  bool leaf;

   public:
  Node(int _t, bool _leaf);

  void insertNonFull(int k);
  void splitChild(int i, Node *y);
  void traverse();

  friend class BTree;
};

class BTree {
  Node *root;
  int t;

   public:
  BTree(int _t) {
    root = NULL;
    t = _t;
  }

  void traverse() {
    if (root != NULL)
      root->traverse();
  }

  void insert(int k);
};

Node::Node(int t1, bool leaf1) {
  t = t1;
  leaf = leaf1;

  key = new int[2 * t - 1];
  child = new Node *[2 * t];

  num_keys = 0;
}

// Traverse the nodes
void Node::traverse() {
  int i;
  for (i = 0; i < num_keys; i++) {
    if (leaf == false)
      child[i]->traverse();
    cout << " " << key[i];
  }

  if (leaf == false)
    child[i]->traverse();
}

// Insert the node


// Insert non full condition


int main() {
  BTree t(3);
  t.insert(8);
  t.insert(9);
  t.insert(10);
  t.insert(11);
  t.insert(15);
  t.insert(16);
  t.insert(17);
  t.insert(18);
  t.insert(20);
  t.insert(23);

  cout << "The B-tree is: ";
  t.traverse();
}