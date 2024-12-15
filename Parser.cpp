#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, sscanf, strcat
#endif

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "scanner.h"
#include "Parser.h"

using namespace std;

// Global variables
// vector<token> allTokens = {
//     {READ, "read"}, {IDENTIFIER, "x"}, {SEMICOLON, ";"},
//     {IF, "if"}, {NUMBER, "0"}, {LESSTHAN, "<"}, {IDENTIFIER, "x"},
//     {THEN, "then"}, {IDENTIFIER, "fact"}, {ASSIGN, ":="}, {NUMBER, "0"}, {SEMICOLON, ";"},
//     {REPEAT, "repeat"}, {IDENTIFIER, "fact"}, {ASSIGN, ":="},
//     {IDENTIFIER, "fact"}, {MULT, "*"}, {IDENTIFIER, "x"}, {SEMICOLON, ";"},
//     {IDENTIFIER, "x"}, {ASSIGN, ":="}, {IDENTIFIER, "x"}, {MINUS, "-"},
//     {NUMBER, "1"}, {UNTIL, "until"}, {IDENTIFIER, "x"}, {EQUAL, "="},
//     {NUMBER, "0"}, {SEMICOLON, ";"}, {WRITE, "write"}, {IDENTIFIER, "fact"}, {END, "end"}
// };

vector<pair<token,int>> allTokens;

int tokenIndex = 0;




void handleError(const string& errorMessage) {
    string s;
    if(tokenIndex == allTokens.size())
        s = to_string(allTokens[tokenIndex-1].second);
    else
        s = to_string(allTokens[tokenIndex].second);
    throw std::runtime_error("Error at line " + s + ": " + errorMessage);
    exit(1);
}

string tokenTypeToString(tokenType type) {
    switch (type) {
    case PLUS: return "PLUS";
    case MINUS: return "MINUS";
    case MULT: return "MULT";
    case DIV: return "DIV";
    case LESSTHAN: return "LESS_THAN";
    case EQUAL: return "EQUAL";
    default: return "UNKNOWN_OPERATOR";
    }
}

Tree* factor() {
    if (tokenIndex >= allTokens.size()) {
        handleError("Unexpected end of input.");
        return nullptr;
    }

    if (allTokens[tokenIndex].first.type == NUMBER) {
        Tree* node = new Tree(NUM_NODE);
        node->num = stoi(allTokens[tokenIndex].first.value);
        tokenIndex++;
        return node;
    } else if (allTokens[tokenIndex].first.type == IDENTIFIER) {
        Tree* node = new Tree(ID_NODE);
        node->id = new char[allTokens[tokenIndex].first.value.size() + 1];
        strcpy(node->id, allTokens[tokenIndex].first.value.c_str());
        tokenIndex++;
        return node;
    } else if (allTokens[tokenIndex].first.type == OPENBRACKET) {
        tokenIndex++; // Consume '('
        Tree* node = exp();
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != CLOSEDBRACKET) {
            handleError("Expected closing bracket ')'.");
        }
        tokenIndex++; // Consume ')'
        return node;
    }

    handleError("Unexpected token : " + allTokens[tokenIndex].first.value);
    return nullptr;
}

Tree* term() {
    if (tokenIndex >= allTokens.size()) {
        handleError("Unexpected end of input.");
        return nullptr;
    }

    Tree* node = factor();
    while (tokenIndex < allTokens.size() &&
           (allTokens[tokenIndex].first.type == MULT || allTokens[tokenIndex].first.type == DIV)) {
        Tree* newNode = new Tree(OPER_NODE);
        newNode->oper = allTokens[tokenIndex].first.type;
        tokenIndex++;
        newNode->child[0] = node;
        newNode->child[1] = factor();
        node = newNode;
    }
    return node;
}

Tree* simple_exp() {
    if (tokenIndex >= allTokens.size()) {
        handleError("Unexpected end of input.");
        return nullptr;
    }

    Tree* node = term();
    while (tokenIndex < allTokens.size() &&
           (allTokens[tokenIndex].first.type == PLUS || allTokens[tokenIndex].first.type == MINUS)) {
        Tree* newNode = new Tree(OPER_NODE);
        newNode->oper = allTokens[tokenIndex].first.type;
        tokenIndex++;
        newNode->child[0] = node;
        newNode->child[1] = term();
        node = newNode;
    }
    return node;
}

Tree* exp() {
    if (tokenIndex >= allTokens.size()) {
        handleError("Unexpected end of input.");
        return nullptr;
    }

    Tree* node = simple_exp();
    if (tokenIndex < allTokens.size() &&
        (allTokens[tokenIndex].first.type == LESSTHAN || allTokens[tokenIndex].first.type == EQUAL)) {
        Tree* newNode = new Tree(OPER_NODE);
        newNode->oper = allTokens[tokenIndex].first.type;
        tokenIndex++;
        newNode->child[0] = node;
        newNode->child[1] = simple_exp();
        node = newNode;
    }
    return node;
}

Tree* stmt() {
    if (tokenIndex >= allTokens.size()) {
        handleError("Unexpected end of input.");
        return nullptr;
    }

    if (allTokens[tokenIndex].first.type == IF) {
        Tree* node = new Tree(IF_NODE);
        tokenIndex++; // Consume IF
        node->child[0] = exp(); // Condition
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != THEN) {
            handleError("Expected THEN after IF.");
        }
        tokenIndex++; // Consume THEN
        node->child[1] = stmt_seq(); // THEN part
        if (tokenIndex < allTokens.size() && allTokens[tokenIndex].first.type == ELSE) {
            tokenIndex++; // Consume ELSE
            node->child[2] = stmt_seq(); // ELSE part
        }
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != END) {
            handleError("Expected END after IF-ELSE statement.");
        }
        tokenIndex++; // Consume END
        return node;
    } else if (allTokens[tokenIndex].first.type == REPEAT) {
        Tree* node = new Tree(REPEAT_NODE);
        tokenIndex++; // Consume REPEAT
        node->child[0] = stmt_seq();
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != UNTIL) {
            handleError("Expected UNTIL after REPEAT.");
        }
        tokenIndex++; // Consume UNTIL
        node->child[1] = exp();
        return node;
    } else if (allTokens[tokenIndex].first.type == IDENTIFIER) {
        Tree* node = new Tree(ASSIGN_NODE);
        node->id = new char[allTokens[tokenIndex].first.value.size() + 1];
        strcpy(node->id, allTokens[tokenIndex].first.value.c_str());
        tokenIndex++; // Consume IDENTIFIER
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != ASSIGN) {
            handleError("Expected := after identifier.");
        }
        tokenIndex++; // Consume ASSIGN
        node->child[0] = exp();
        return node;
    } else if (allTokens[tokenIndex].first.type == READ) {
        Tree* node = new Tree(READ_NODE);
        tokenIndex++; // Consume READ
        if (tokenIndex >= allTokens.size() || allTokens[tokenIndex].first.type != IDENTIFIER) {
            handleError("Expected identifier after READ.");
        }
        node->child[0] = new Tree(ID_NODE);
        node->child[0]->id = new char[allTokens[tokenIndex].first.value.size() + 1];
        strcpy(node->child[0]->id, allTokens[tokenIndex].first.value.c_str());
        tokenIndex++;
        return node;
    } else if (allTokens[tokenIndex].first.type == WRITE) {
        Tree* node = new Tree(WRITE_NODE);
        tokenIndex++; // Consume WRITE
        node->child[0] = exp();
        return node;
    }

    handleError("Unexpected token : " + allTokens[tokenIndex].first.value);
    return nullptr;
}

Tree* stmt_seq() {
    Tree* node = stmt();
    Tree* root = node;
    while (tokenIndex < allTokens.size() && allTokens[tokenIndex].first.type == SEMICOLON) {
        tokenIndex++;
        Tree* sibling = stmt();
        node->sibling = sibling;
        node = sibling;
    }

    return root;
}

void printSyntaxTree(Tree* tree, int depth) {
    if (!tree) return;
    for (int i = 0; i < depth; i++) cout << "  ";
    switch (tree->node_kind) {
    case NUM_NODE: cout << "NUM: " << tree->num << endl; break;
    case ID_NODE: cout << "ID: " << tree->id << endl; break;
    case OPER_NODE: cout << "OPER: " << tokenTypeToString(tree->oper) << endl; break;
    case IF_NODE: cout << "IF" << endl; break;
    case REPEAT_NODE: cout << "REPEAT" << endl; break;
    case ASSIGN_NODE: cout << "ASSIGN to: " << tree->id << endl; break;
    case READ_NODE: cout << "READ" << endl; break;
    case WRITE_NODE: cout << "WRITE" << endl; break;
    default: cout << "Unknown Node" << endl; break;
    }
    for (int i = 0; i < 3; i++) printSyntaxTree(tree->child[i], depth + 1);
    printSyntaxTree(tree->sibling, depth);
}

