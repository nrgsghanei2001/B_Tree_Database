#include<iostream>
#include<vector>
#include<cmath>
#include"BTree.h"
using namespace std;


int main() {
    int t = 5;
    BTree tree(t);
    int choice = 1;
    while (choice != 4) {
        cin >> choice;
        if (choice == 1) {       // insert
            int num;
            cin >> num;
            tree.Insert(num);
            cout << "---------------"<<endl;
            tree.traverse();
            cout << "---------------" << endl;
        }
        else if (choice == 2) {      // delete
            int num;
            cin >> num;
            tree.Delete(num);
            cout << "---------------"<<endl;
            tree.traverse();
            cout << "---------------" << endl; 
        }
        else if (choice == 3) {      // search
            int num;
            cin >> num;
            Node* res = tree.Search(num);
            cout << "---------------"<<endl;
            if (res) {
                cout << "OK" << endl;
            }
            else {
                cout << "Not found" << endl;
            }
            cout << "---------------" << endl; 
        }
    }
     
    return 0;
}