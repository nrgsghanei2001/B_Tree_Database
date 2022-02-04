#include<iostream>
#include<bits/stdc++.h>
#include<vector>
#include<string>
#include"BTree.h"
using namespace std;


class Query {
    private:
        vector<string> tokens;
    public:
        void parse_query(string);
        vector<string> get_tokens();
};
//////////////////////////////////////////////////////////
void Query::parse_query(string input) {
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
    public:
        Table(string);
        string get_name();
        void create_filed(string, string);
        void show_fields();
        void set_id(int);
        int get_id();
};
////////////////////////////////////////////////////////
Table::Table(string name) {
    table_name = name;
    fields.push_back({});
    fields.push_back({});
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

                // add fields of table to it
                for (int i = 3; i < tokens.size(); i+=2) {
                    table_obj->create_filed(tokens[i], tokens[i + 1]);
                }
            }
        }
        else if (operation == "INSERT") {
            string into = tokens[1];    // second token should be INTO
            if (into == "INTO") {
                string table_name = tokens[2];         // third token should be name of table we wanna insert to
            }
        }
        else if (operation == "DELETE") {
            string from = tokens[1];    // second token should be FROM
            if (from == "FROM") {
                string table_name = tokens[2];         // third token should be name of table we wanna delete from
            }
        }
        else if (operation == "SELECT") {

        }
        else if (operation == "UPDATE") {

        }
    }
     
    for (int i = 0; i < all_tables.size(); i++) {
        cout << all_tables[i]->get_name() << endl;
        cout << all_tables[i]->get_id() << endl;
        all_tables[i]->show_fields();
    }

    return 0;
}