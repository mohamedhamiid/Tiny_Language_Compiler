#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

using namespace std;

// Enum to define token types
typedef enum
{
    SEMICOLON,
    IF,
    THEN,
    END,
    REPEAT,
    UNTIL,
    IDENTIFIER,
    ASSIGN,
    READ,
    WRITE,
    LESSTHAN,
    EQUAL,
    PLUS,
    MINUS,
    MULT,
    DIV,
    OPENBRACKET,
    CLOSEDBRACKET,
    NUMBER,
    ELSE
} tokenType;

// Struct to represent a token with its type and value
typedef struct
{
    tokenType type; // Type of the token
    string value;   // Actual string value of the token
} token;

// Define an error token type for invalid tokens
#define ERROR (tokenType) - 1

typedef enum {
    LETTER,
    SYMBOL,
    DIGIT,
    WHITESPACE,
    NOTEXIST
}type;

vector<pair<token,int>> scanner(string in);
token detectString(string in, int line);
token detectNonLetter(string str, int line);
type getType(char in);
#endif // SCANNER_H
