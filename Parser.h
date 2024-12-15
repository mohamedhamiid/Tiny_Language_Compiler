#ifndef PARSER_H
#define PARSER_H


enum NodeKind {
    IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
    OPER_NODE, NUM_NODE, ID_NODE, STMT_SEQ_NODE
};

struct Tree {
    Tree* child[3] = {nullptr, nullptr, nullptr};
    Tree* sibling = nullptr;

    NodeKind node_kind;
    union { tokenType oper; int num; char* id; };
    int line_num;

    Tree(NodeKind kind) : node_kind(kind) {}
};

// Function Prototypes
Tree* factor();
Tree* term();
Tree* simple_exp();
Tree* exp();
Tree* stmt();
Tree* stmt_seq();
void printSyntaxTree(Tree* tree, int depth = 0);
string tokenTypeToString(tokenType type);
void handleError(const string& errorMessage);

#endif // PARSER_H
