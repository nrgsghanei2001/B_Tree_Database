#include "Table.h"



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
            else {
                cout << "Syntax Error! command " << table << " not found. Did you mean TABLE?" << endl;
                exit(0);
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
                    cout << "Table not found" << endl;
                    exit(0);
                }
                // table found
                else {
                    table_obj->insertion(tokens);
                }
            }
            else {
                cout << "Syntax Error! command " << into << " not found. Did you mean INTO?" << endl;
                exit(0);
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
                    cout << "Table not found" << endl;
                    exit(0);
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
            else {
                cout << "Syntax Error! command " << from << " not found. Did you mean FROM?" << endl;
                exit(0);
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
                cout << "Table not found" << endl;
                exit(0);
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
                    cout << "Table not found" << endl;
                }
                // table found
                else {
                    vector<vector<string>> all_selects;
                    if (tokens[tokens.size() - 2] == "|") {
                        all_selects = table_obj->select_or(tokens, table_ind);
                    }
                    else if (tokens[tokens.size() - 2] == "&") {
                        all_selects = table_obj->select_and(tokens, table_ind);
                    }
                    else {
                        all_selects = table_obj->select(tokens, table_ind);
                    }
                    for (int k = 0; k < all_selects.size(); k++) {                  // add result of query to all thing we should show at last
                        all_select_queries.push_back(all_selects[k]);
                    }
                }
            }
            else {
                cout << "Syntax Error! command not found. Did you mean FROM?" << endl;
                exit(0);
            }
        }
        else {
            cout << "Syntax Error! command " <<  operation << " not found." << endl;
            exit(0);
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