#include <vector>
#include "BTree.h"
#include "Parser.h"
#include <iostream>
#include <string>
using namespace std;

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
        vector<vector<string>> select_or(vector<string>, int);                                    // get all records that contain condition for |
        vector<vector<string>> select_and(vector<string>, int);                                   // get all records that contain condition for &
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
    else {
        cout << "Invalid type!" << endl;
        exit(0);
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
                    exit(0);
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
                    exit(0);
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
                    exit(0);
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
                        exit(0);
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
                        exit(0);
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
                        exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
                exit(0);
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
vector<vector<string>> Table::select_or(vector<string> tokens, int table_ind) {
    vector<int> selected_columns;                               // columns we want their entries
    int num_sel = table_ind - 2;                                // number of columns we want their data for result
    int pos_tok1      =  table_ind + 2;                         // find the index of condition
    int pos_tok2      =  table_ind + 4;                         // find the index of condition
    string condition1 = tokens[pos_tok1];
    string condition2 = tokens[pos_tok2];
    // find answer of each condition seprately
    vector<string> tokens1, tokens2;
    vector<vector<string>> result1, result2, answers, final_answers;
    tokens1.push_back("SELECT");
    tokens1.push_back("*");
    tokens1.push_back("FROM");
    tokens1.push_back(tokens[table_ind]);
    tokens1.push_back("WHERE");
    tokens1.push_back(condition1);
    result1 = select(tokens1, 3);
    tokens2.push_back("SELECT");
    tokens2.push_back("*");
    tokens2.push_back("FROM");
    tokens2.push_back(tokens[table_ind]);
    tokens2.push_back("WHERE");
    tokens2.push_back(condition2);
    result2 = select(tokens2, 3);
    // merge the results
    for (int i = 0; i < result2.size(); i++) {
        int flag = 1;
        for (int j = 0; j < result2.size(); j++) {
            if (result1[i][0] == result2[j][0]) {
                flag = 0;
                break;
            }
        }
        if (flag)
            result1.push_back(result2[i]);
    }

    // sort the result base on id
    vector<string> min;
    for (int i = 0; i < result1.size(); i++) {
        for (int j = 0; j < result1.size(); j++) {
            if (result1[i][0] < result1[j][0]) {
                min = result1[i];
                result1[i] = result1[j];
                result1[j] = min;
            }
        }
    }

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

    // only chosse the requested fields of answers
    for (int i = 0; i < result1.size(); i++) {
        answers.push_back({});

        for (int j = 0; j < columns.size(); j++) {
            for (int k = 0; k < selected_columns.size(); k++) {
                if (selected_columns[k] == j) {                       // got to one of selected columns
                    answers[i].push_back(result1[i][j]);
                }
            }
        }

    }

    return answers;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<string>> Table::select_and(vector<string> tokens, int table_ind) {
    vector<int> selected_columns;                               // columns we want their entries
    int num_sel = table_ind - 2;                                // number of columns we want their data for result
    int pos_tok1      =  table_ind + 2;                         // find the index of condition
    int pos_tok2      =  table_ind + 4;                         // find the index of condition
    string condition1 = tokens[pos_tok1];
    string condition2 = tokens[pos_tok2];
    // find answer of each condition seprately
    vector<string> tokens1, tokens2;
    vector<vector<string>> result1, result2, answers, final_answers;
    tokens1.push_back("SELECT");
    tokens1.push_back("*");
    tokens1.push_back("FROM");
    tokens1.push_back(tokens[table_ind]);
    tokens1.push_back("WHERE");
    tokens1.push_back(condition1);
    result1 = select(tokens1, 3);
    tokens2.push_back("SELECT");
    tokens2.push_back("*");
    tokens2.push_back("FROM");
    tokens2.push_back(tokens[table_ind]);
    tokens2.push_back("WHERE");
    tokens2.push_back(condition2);
    result2 = select(tokens2, 3);

    // choose the results which are repeated in both conditions
    for (int i = 0; i < result1.size(); i++) {
        for (int j = 0; j < result2.size(); j++) {
            if (result1[i][0] == result2[j][0]) {
                final_answers.push_back(result1[i]);
                break;
            }
        }
    }
    result1 = final_answers;

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

    // only chosse the requested fields of answers
    for (int i = 0; i < result1.size(); i++) {
        answers.push_back({});

        for (int j = 0; j < columns.size(); j++) {
            for (int k = 0; k < selected_columns.size(); k++) {
                if (selected_columns[k] == j) {                       // got to one of selected columns
                    answers[i].push_back(result1[i][j]);
                }
            }
        }

    }

    return answers;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
