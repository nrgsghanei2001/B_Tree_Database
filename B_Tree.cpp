#include<iostream>
using namespace std;


// Class Node for each element of B tree
class Node {
public:
    int data;
    Node* nextField;
    BTNode* self;
};
/////////////////////////////////////////////////////
// Class B tree with all operations on tree
class BTNode {
    private:
        Node* root;

    public:
        Node* Search(int);
        Node* Insert(int);
        Node* Delete(int);
        Node* Update(int);
        void Traverse();

};
//////////////////////////////////////////////////////
