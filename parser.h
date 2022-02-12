#include <iostream>
#include <vector>
#include <cmath>
using namespace std;


// This function hashes strings input containing small letters and numbers
long long int hash_string(string s) {
    // hash table for saving code of each character
    char hash_table[37] = {'/','0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    
    long long int hashed = 0;
    for (int i = 0; i < s.length(); i++) {
        char ch = s[i];
        int  h  = 0;
        for (int j = 0; j < 37; j++) {
            if (ch == hash_table[j]) {
                h = j;                      // find character in table
                break;
            }
        }
        hashed += h * floor(pow(37, i));   // add to result
    }
    return hashed;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// Convert the hashed number to string
string dehash_string(long long int hashed) {
    // hash table for saving code of each character
    char hash_table[37] = {'/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    string dehash = "";
    // turn to base 37 and find the answer
    while (hashed != 0) {
        int d   = hashed % 37;
        dehash += hash_table[d];
        hashed /= 37;
    }
    return dehash;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function hashes date input
long long int hash_date(string date) {
    long long int h = 0;
    int first       = 0,
    second          = 0;            
    // keeps the index of first and second / in date
    for (int i = 0; i < date.length(); i++) {
        if (date[i] == '/' && !first) {
            first = i;
        }
        else if (date[i] == '/' && !second) {
            second = i;
        }
    }
    string year  = date.substr(0, 4);
    string month = date.substr(first + 1, second - first - 1);
    string day   = date.substr(second + 1);
    // convert the number od year, month and day to int
    h += stoi(year);
    h *= 100;
    h += stoi(month);
    h *= 100;
    h += stoi(day);

    return h;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// Convert hashed number to date object
string dehash_date(long long int d) {
    string dehash = "";
    string date   = to_string(d);
    string year   = date.substr(0, 4);     // 4 first number is year
    string month  = date.substr(4, 2);     // 2 second number is month
    string day    = date.substr(6, 2);     // 2 third number is day
    dehash        += year;
    dehash        += '/';

    if (month[0] == '0') {                  // if month is single digit ignore the ziro
        dehash += month[1];
    }
    else {
        dehash += month;
    }
    dehash += '/';
    if (day[0] == '0') {                   // if day is single digit ignore the ziro
        dehash += day[1];
    }
    else {
        dehash += day;
    }

    return dehash;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input string to be valid (small letters and numbers)
bool check_string_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        // not small letter
        if (int(s[i]) < 97 || int(s[i]) > 122) {
            // not number
            if (!(int(s[i]) >= 48 && int(s[i]) <= 57)) {
                return false;
            }
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input date to be valid
bool check_date_validation(string d) {
    if (d.length() > 10 || d.length() < 8) {
        return false;             // not enough or more than enough characters
    }
    int slash = 0, f_slash, s_slash;
    // find the index of /'s in date
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
        return false;       // date should contaion 2 /'s and first slash should be in 4'th place
    }
    if (!(s_slash == 6 || s_slash == 7)) {
        return false;       // second slash should be in 6 or 7'th place
    }
    // invalid characters
    for (int i = 0; i < d.length(); i++) {
        if (int(d[i]) > 57 || int(d[i]) < 47) {
            return false;
        }
    }
    // find the month and check it be between 1 and 12
    string month = d.substr(5, s_slash - f_slash);
    int    mon   = stoi(month);
    if (mon < 1 || mon > 12) {
        return false;
    }
    // find the day and check it be between 1 and 31
    string day = d.substr(s_slash + 1);
    int    dd  = stoi(day);
    if (dd < 1 || dd > 31) {
        return false;
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks the input int to be valid
bool check_int_validation(string s) {
    for (int i = 0; i < s.length(); i++) {
        // invalid charcters (not between 0 to 9)
        if (int(s[i]) < 48 || int(s[i]) > 57) {
            return false;
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
vector<string> parse_query(string input) {
    string inputed = "";
    // delete "" from strings
    for (int i = 0; i < input.length(); i++) {
        if (int(input[i]) != 34) {
            inputed += input[i];
        }
    }

    input       = inputed;
    string word = "";
    vector<string> tokens;                         // save all of tokens 
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
    return tokens;
}
///////////////////////////////////////////////////////////////////////////////////////////////////