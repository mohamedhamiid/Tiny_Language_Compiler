#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <QString>

#include "scanner.h"

using namespace std;

char error = 0;
// Array to map token types to their string representation
QString Types[]{
               "SEMICOLON",
               "IF",
               "THEN",
               "END",
               "REPEAT",
               "UNTIL",
               "IDENTIFIER",
               "ASSIGN",
               "READ",
               "WRITE",
               "LESSTHAN",
               "EQUAL",
               "PLUS",
               "MINUS",
               "MULT",
               "DIV",
               "OPENBRACKET",
               "CLOSEDBRACKET",
               "NUMBER",
               "ELSE"
};

type getType(string in) {
    if (regex_match(in, regex("[a-zA-Z]+")))
        return LETTER;
    else if (regex_match(in, regex("^[0-9]+$")))
        return DIGIT;
    else if (regex_match(in, regex("[;:=<+\\-*/()/]")))
        return SYMBOL;
    else if (regex_match(in, regex("^\\s+$")))
        return WHITESPACE;
    else
        return NOTEXIST;
}

// Function to detect and classify non-letter tokens (symbols, numbers, etc.)
token detectNonLetter(string str, int line)
{
    token res;
    if (str == "<")
    {
        res.value = str;
        res.type = LESSTHAN;
    }
    else if (str == "=")
    {
        res.value = str;
        res.type = EQUAL;
    }
    else if (str == "+")
    {
        res.value = str;
        res.type = PLUS;
    }
    else if (str == "-")
    {
        res.value = str;
        res.type = MINUS;
    }
    else if (str == "*")
    {
        res.value = str;
        res.type = MULT;
    }
    else if (str == "/")
    {
        res.value = str;
        res.type = DIV;
    }
    else if (str == ":=")
    {
        res.value = str;
        res.type = ASSIGN;
    }
    else if (str == "(")
    {
        res.value = str;
        res.type = OPENBRACKET;
    }
    else if (str == ")")
    {
        res.value = str;
        res.type = CLOSEDBRACKET;
    }
    else if (regex_match(str, regex("^[0-9]+$"))) // Check if the string is a number
    {
        res.value = str;
        res.type = NUMBER;
    }
    else if (str == ";") {
        res.value = str;
        res.type = SEMICOLON;
    }
    else // If no match, classify as an error
    {
        res.type = ERROR;
        cout << "  Error at line " << line << " (" << str << ')' << endl;
        res.value = str;
    }
    return res;
}

// Function to detect and classify string tokens (keywords and identifiers)
token detectString(string in, int line)
{
    token res;
    if (in == "if")
        res.type = IF;
    else if (in == "then")
        res.type = THEN;
    else if (in == "repeat")
        res.type = REPEAT;
    else if (in == "end")
        res.type = END;
    else if (in == "until")
        res.type = UNTIL;
    else if (in == "read")
        res.type = READ;
    else if (in == "write")
        res.type = WRITE;
    else if (in == "else")
        res.type = ELSE;
    else
    {
        res.type = IDENTIFIER; // Default to identifier
        int len = in.length();
        for (int i = 0; i < len; i++)
        {
            // Ensure all characters are alphabetic; if not, mark as error
            if ((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z'))
                ;
            else
            {
                res.type = ERROR;
                cout << "  Error at line " << line << " (" << in << ')' << endl;
                res.value = in;
                break;
            }
        }
    }
    if (res.type != ERROR)
        res.value = in;
    return res;
}
char flag=1;
// Scanner function to tokenize the input
vector<pair<token,int>> scanner(string in)
{
    char comment = 0;
    for (int i = 0; i < in.length(); i++)
    {
        if (in[i] == '{')
            comment = 1;
        if (in[i] == '}') {
            comment = 0;
            in[i] = ' ';
        }
        if (comment) {
            in[i] = ' ';
        }
    }
    vector<pair<token,int>> tokens; // Vector to store the tokens

    stringstream check1(in); // Create a stringstream for the input string

    string line;
    int lines = 0; // Track the line number
    while (getline(check1, line, '\n')) // Process the input line by line
    {
        lines++;
        flag=1;

        string word;

        token res;

        string ch;

        int i = 0, j = 1;
        while (j<line.length()) {

            while (j < line.length() && (getType(string(1,line[i])) == getType(string(1,line[j]))) && getType(string(1,line[i]))!= SYMBOL) j++;

            string word;
            if (i != j)
                word = line.substr(i, j-i);
            else
                word = line.substr(i, 1);


            if (getType(string(&word[0])) == WHITESPACE);
            else if (getType(string(&word[0])) != NOTEXIST) {
                if (getType(string(&word[0])) == LETTER)
                    res = detectString(word, lines); // Classify as a string token
                else if (getType(string(&word[0])) == DIGIT)
                    res = detectNonLetter(word, lines); // Classify as a string token
                else if (getType(string(&word[0])) == SYMBOL) {
                    if (word == ":" && line[j] == '=') {
                        word.push_back('=');
                        j++;
                    }
                    res = detectNonLetter(word, lines); // Classify as a string token
                }
                tokens.push_back(make_pair(res,lines));
            }
            else {
                cout << "  Error at line " << lines << " (" << word << ')' << endl;
                error = 1;
                break;
            }

            i = j;
            j++;
            if(flag && j==line.length()){
                j--;
                flag=0;
            }

        }
        if(error)
            break;
    }
    return tokens;
}
