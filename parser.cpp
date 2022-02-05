#include<iostream>
#include<bits/stdc++.h>
#include<vector>
#include<string>
#include<typeinfo>
#include"BTree.h"
using namespace std;

long long int hash_string(string s) {
    long long int h = 0;
    for (int i = 0; i < s.length(); i++) {
        h += int(s[i]) * (i + 1);
    }
    return h;
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
    // for(int i = 0; i < tokens.size(); i++)
    //     cout << tokens[i] << '\n';
}
////////////////////////////////////////////////////////
vector<string> Query::get_tokens() {
    return tokens;
}
////////////////////////////////////////////////////////
class Table {
    private:
        string table_name;
        int table_id;
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
        long long int find_id_to_insert();
        void show_columns_info();
        void deletion(vector<string>);
        void update(vector<string>);
        void select(vector<string>, int);
        void check_for_equal(BTNode*, long long int, vector<Node*>&);
        void check_for_greater(BTNode*, long long int, vector<Node*>&);
        void check_for_smaller(BTNode*, long long int, vector<Node*>&);
};
////////////////////////////////////////////////////////
Table::Table(string name) {
    table_name = name;
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
        cout << "Blank field is not allowed, Please try again.";
        return;
    }
    else {
        vector<Node*> inserted_list;
        long long int id = find_id_to_insert();
        Node* insertt = columns[0]->Insert(id);
        // cout << insertt->data << " *** ";
        id_table[id] = false;
        // inserted_list.push_back(columns[0]->Search(id));
        inserted_list.push_back(insertt);
        long long int h;
        
        for (int i = 4; i < tokens.size(); i++) {
            string type_of = fields[1][i - 3];
            // convert to right type
            if (type_of == "string") {
                h = hash_string(tokens[i]);
            }
            else if (type_of == "timestamp") {
                h = hash_date(tokens[i]);
            }
            else {
                h = stoll(tokens[i]);
            }
            insertt = columns[i - 3]->Insert(h);
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
        //     cout << node->nextField->data << " ";
        //     node = node->nextField;
        // }
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
                cout << "Invalid condition! Please Try again." << endl;
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
            
            for (int i = 0; i < all_equal_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_equal_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    bt->Delete2(all_equal_nodes[i]);   // delete the node
                    bt->traverse();
                    all_equal_nodes[i] = all_equal_nodes[i]->nextField;
                    col++;
                }
            }
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            vector<Node*> all_greater_nodes;
            check_for_greater(rt, h, all_greater_nodes);       // find all nodes that are greater than statement
            
            for (int i = 0; i < all_greater_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        id_table[all_greater_nodes[i]->data] = true;      // free the deleted nodes id
                    }

                    bt->Delete2(all_greater_nodes[i]);   // delete the node
                    bt->traverse();
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
                        id_table[all_smaller_nodes[i]->data] = true;      // free the deleted nodes id
                    }
                    // cout << "delete " << all_equal_nodes[i]->data << endl;
                    bt->Delete2(all_smaller_nodes[i]);   // delete the node
                    bt->traverse();
                    all_smaller_nodes[i] = all_smaller_nodes[i]->nextField;
                    col++;
                }
            }
        }
    }
    // column not found
    else {
        cout << "There is not such a column in this table, please try again." << endl;
        return;
    }

}
//////////////////////////////////////////////////////
void Table::update(vector<string> tokens) {
    int pos_tok =  columns.size() + 3;
    // cout << pos_tok << endl;
    string condition = tokens[pos_tok], fields_name, operation, state;
    // cout << condition << endl;
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
                cout << "Invalid condition! Please Try again." << endl;
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
        long long int h, h2;
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
        // what to change
        vector<string> change;
        vector<long long int> hash_change;
        vector<int> ids;
        for (int i = 3; i < columns.size() + 2; i++) {
            change.push_back(tokens[i]);
        }
        for (int i = 0; i < change.size(); i++) {
            if (fields[1][i + 1] == "string") {
                h2 = hash_string(change[i]);
            }
            else if (fields[1][i + 1] == "timestamp") {
                h2 = hash_date(change[i]);
            }
            else if (fields[1][i + 1] == "int") {
                h2 = stoll(change[i]);
            }
            hash_change.push_back(h2);
        }
        for (int i = 0; i < hash_change.size(); i++) cout << hash_change[i] << " * ";
        cout << endl;
        vector<Node*> all_nodes;
        // do conditions
        if (operation == "==") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_equal(rt, h, all_nodes);       // find all nodes that are equal to statement
            
            for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
                    }

                    bt->Delete2(all_nodes[i]);   // delete the node
                    // bt->traverse();
                    // cout << endl <<"-----------"<<endl;
                    all_nodes[i] = all_nodes[i]->nextField;
                    col++;
                }
            }
            
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            // cout << "h: "<<h <<endl;
            check_for_greater(rt, h, all_nodes);       // find all nodes that are equal to statement
            // for (int i = 0; i < all_nodes.size(); i++) cout << all_nodes[i]->data << "++"<<endl;
            for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
                    }

                    bt->Delete2(all_nodes[i]);   // delete the node
                    // bt->traverse();
                    // cout << endl <<"-----------"<<endl;
                    all_nodes[i] = all_nodes[i]->nextField;
                    col++;
                }
            }
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_smaller(rt, h, all_nodes);       // find all nodes that are equal to statement
            
            for (int i = 0; i < all_nodes.size(); i++) {   // for all nodes that had the condition
                for (int j = 0; j < columns.size(); j++) {        // delete all entries of them
                    BTree* bt = columns[col % columns.size()];
                    if (fields[0][col % columns.size()] == "id") {
                        ids.push_back(all_nodes[i]->data);      // save the deleted nodes id to get to new node
                    }

                    bt->Delete2(all_nodes[i]);   // delete the node
                    // bt->traverse();
                    // cout << endl <<"-----------"<<endl;
                    all_nodes[i] = all_nodes[i]->nextField;
                    col++;
                }
            }
            
        }
        // insert updated datas to tree
        for (int i = 0; i < all_nodes.size(); i++) {    // for the exact number of deleted nodes
            vector<Node*> inserted_list;
            long long int id = ids[i];
            Node* insertt = columns[0]->Insert(id);
            inserted_list.push_back(insertt);
            for (int j = 0; j < hash_change.size(); j++) {
                // cout << "insert: "<<hash_change[j]<<endl;
                insertt = columns[j + 1]->Insert(hash_change[j]);
                // cout << insertt->data << endl;
                inserted_list.push_back(insertt);
            }
            // fix next_fields of nodes
            for (int j = 0; j < columns.size() - 1; j++) {
                inserted_list[j]->nextField = inserted_list[j + 1];
                cout << inserted_list[j]->data << " " << inserted_list[j]->nextField->data << endl;
            }
            inserted_list[columns.size() - 1]->nextField = inserted_list[0];
            cout << inserted_list[columns.size() - 1]->data << " " << inserted_list[columns.size() - 1]->nextField->data << endl;
        }
        cout << "gettt";
        return;
    }
    // column not found
    else {
        cout << "There is not such a column in this table, please try again." << endl;
        return;
    }
}
/////////////////////////////////////////////////////
void Table::check_for_equal(BTNode* node, long long int h, vector<Node*> & change) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // cout << "ding" << endl;
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_equal(node->child[i], h, change);
        // cout << node->key[i]->data << " ";
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
        // cout << "ding" << endl;
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_equal(node->child[i], h, change);
        // cout << node->key[i]->data << " ";
        if (node->key[i]->data > h) {
            change.push_back(node->key[i]);
        }
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        check_for_equal(node->child[i], h, change);
    }
}
//////////////////////////////////////////////////////////////////////////////
void Table::check_for_smaller(BTNode* node, long long int h, vector<Node*> & change) {
    long long int i;

    for (i = 0; i < node->num_keys; i++) {
        // cout << "ding" << endl;
        // If this is not leaf, then before key[i]
        // traverse the subtree rooted with child C[i]
        if (node->is_leaf == false)
            check_for_equal(node->child[i], h, change);
        // cout << node->key[i]->data << " ";
        if (node->key[i]->data < h) {
            change.push_back(node->key[i]);
        }
    }
 
    // subtree rooted with last child
    if (node->is_leaf == false) {
        check_for_equal(node->child[i], h, change);
    }
}
//////////////////////////////////////////////////////////////////////////////
void Table::select(vector<string> tokens, int table_ind) {
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
    // cout << condition << endl;
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
                cout << "Invalid condition! Please Try again." << endl;
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

        vector<Node*> all_nodes;
        // do conditions
        if (operation == "==") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_equal(rt, h, all_nodes);       // find all nodes that are equal to statement
        }
        else if (operation == ">") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_greater(rt, h, all_nodes);       // find all nodes that are equal to statement
        }
        else if (operation == "<") {
            BTree* bt = columns[col % columns.size()];
            BTNode* rt = bt->get_root();
            check_for_smaller(rt, h, all_nodes);       // find all nodes that are equal to statement
        }
        vector<vector<long long int>> answers;
        vector<Node*> sorted1, sorted2;
        sorted1 = all_nodes;
        // vector<Node*> sorted1, sorted2;
        // for (int i = 0; i < all_nodes.size(); i++) {
        //     Node* move = all_nodes[i];
        //     int cur_col = col;
            
        //     while (cur_col % columns.size() != 0) {
        //         move = move->nextField;
        //         cur_col++;
        //     }
        //     sorted1.push_back(move);
        // }
        // for (int i = 0; i < sorted1.size(); i++) {
        //     for (int j = i + 1; j < sorted1.size(); j++) {
        //         if (sorted1[i]->data > sorted1[j]->data) {
        //             Node* temp = sorted1[i];
        //             sorted1[i] = sorted1[j];
        //             sorted1[j] = temp;
        //         }
        //     }
        // }
        for (int i = 0; i < sorted1.size(); i++) {
            answers.push_back({});
            Node* move = sorted1[i];
            int cur_col = col;
            
            while (cur_col % columns.size() != 0) {
                move = move->nextField;
                cur_col++;
            }
            // cout << "CUR: " << cur_col<<endl;
            for (int j = 0; j < columns.size(); j++) {
                for (int k = 0; k < selected_columns.size(); k++) {
                    if (selected_columns[k] == j) {
                        answers[i].push_back(move->data);
                        break;
                    }
                }
                move = move->nextField;
            }
            // for (int j = 0; j < columns.size(); j++) {
            //     cout << move->data << " ";
            //     move = move->nextField;
            // }
            // cout << endl<<"/////"<<endl;
        }
        for (int i = 0; i< answers.size(); i++) {
            for (int j = 0; j < answers[i].size(); j++) {
                cout << answers[i][j] << " ";
            }
            cout << endl;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
int main() {
    string input;
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
                    cout << "Sorry! there is not such a table, Please try again." << endl;
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
                    cout << "Sorry! there is not such a table, Please try again." << endl;
                }
                // table found
                else {
                    table_obj->deletion(tokens);
                }

            }
            
        }
        else if (operation == "UPDATE") {
            string table_name = tokens[1];         // second token should be name of table we wanna update 
            // find the table object
            Table* table_obj = NULL;
            for (int i = 0; i < all_tables.size(); i++) {
                if (all_tables[i]->get_name() == table_name) {
                    // cout << "HERE"<<endl;
                    table_obj = all_tables[i];
                    break;
                }
            }
            // the table not exists
            if (!table_obj) {
                cout << "Sorry! there is not such a table, Please try again." << endl;
            }
            // table found
            else {
                // cout << "HERE" << endl;
                table_obj->update(tokens);
                cout << "KK";
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
                    cout << "Sorry! there is not such a table, Please try again." << endl;
                }
                // table found
                else {
                    table_obj->select(tokens, table_ind);
                }
            }
        }
    }
     
    for (int i = 0; i < all_tables.size(); i++) {
        // cout << "got";
        cout << all_tables[i]->get_name() << endl;
        cout << all_tables[i]->get_id() << endl;
        all_tables[i]->show_fields();
        all_tables[i]->show_columns_info();
    }

    return 0;
}