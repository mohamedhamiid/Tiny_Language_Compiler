#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <string>
#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <QMessageBox>
#include <QString>
#include <QPair>

#include "scanner.h"
#include "Parser.h"

using namespace std;

vector<pair<int,bool>> lastX;

int padding = 10;

string label[]={
    "if", "repeat", "assign\n", "read\n", "write",
    "op\n", "const\n", "id\n", "STMT_SEQ_NODE"
};

string op[16];

void drawTree(QGraphicsScene* scene, Tree* node, int x, int y, int xOffset, int yStep, int childXoffset,int level) {
    if (!node) return;

    if(level&&lastX[level].second){
        lastX[level].second = 0;
        lastX[level].first = lastX[level-1].first;
    }
    // Calculate node dimensions based on text length
    int textWidth = label[node->node_kind].size() * 8; // Approximate width per character
    //int nodeWidth = textWidth + 20; // Add padding
    int nodeWidth = 100;
    int nodeHeight = 40;

    // Draw the current node
    if(node->node_kind == OPER_NODE || node->node_kind == NUM_NODE || node->node_kind == ID_NODE)
        QGraphicsEllipseItem* ellipse = scene->addEllipse(x, y, nodeWidth, nodeHeight);
    else
        QGraphicsRectItem* rect = scene->addRect(x, y, nodeWidth, nodeHeight);

    QGraphicsTextItem* text;

    string s;
    if(node->node_kind == NUM_NODE){
        s = label[node->node_kind];
        s.push_back('(');
        s = s + to_string(node->num);
        s.push_back(')');

    }
    else if(node->node_kind == IF_NODE || node->node_kind == REPEAT_NODE ||node->node_kind == WRITE_NODE){
        s = label[node->node_kind];
    }
    else if(node->node_kind == READ_NODE){
        s = label[node->node_kind];
        s.append((node->child[0]->id));
    }
    else if(node->node_kind == OPER_NODE){
        op[10] = "<";
        op[11] = "=";
        op[12] = "+";
        op[13] = "-";
        op[14] = "*";
        op[15] = "/";
        s = label[node->node_kind];
        s.push_back('(');
        s.append(op[node->oper]);
        s.push_back(')');
    }
    else{
        s = label[node->node_kind];
        s.push_back('(');
        s.append((node->id));
        s.push_back(')');
    }
    //cout<< s<<endl;
    text = scene->addText(QString::fromStdString(s));
    text->setPos(x + (nodeWidth - textWidth) / 2, y +1);

    lastX[level].first = x + nodeWidth + padding;

    // Draw children
    // Configure dashed line pen
    QPen dashedPen(Qt::DashLine);

    if(node->node_kind != READ_NODE){
        for (int i = 0; i < 3; ++i) {
            if (node->child[i]) {

                if(level<lastX.size()-1&&lastX[level+1].second){
                    lastX[level+1].second = 0;
                    lastX[level+1].first = lastX[level].first-200;
                }

                int childX = max(lastX[level+1].first,(lastX[level].first-200)); // Start slightly to the left if there are children
                cout<< "Last Value at Level "<<level<<" "<<lastX[level].first<<" "<< label[node->node_kind]<< " " << x<<endl ;

                if(i&&(node->child[i-1]->sibling!=NULL)){
                    childX = childX + xOffset - 150;
                }

                int childY = y + yStep;
                int childWidth = label[node->child[i]->node_kind].size() * 8 + 20; // Calculate child's width
                QGraphicsLineItem* line = scene->addLine(x + nodeWidth / 2, y + nodeHeight,
                                                         childX + childWidth /1.25, childY);

                if(i==2 && node->node_kind == IF_NODE)
                    line->setPen(dashedPen); // Set dashed line style

                drawTree(scene, node->child[i], childX, childY, xOffset, yStep,childXoffset,level+1);
                //childX += childXoffset * 4; // Adjusted horizontal spacing between children
                childX = lastX[level].first;
            }
        }
    }

    // Draw sibling
    if (node->sibling) {
        int siblingX = max(x + xOffset * 1,lastX[level+1].first); // Increased space between siblings
        QGraphicsLineItem* line = scene->addLine(x + nodeWidth, y + nodeHeight / 2,
                                                 siblingX, y + nodeHeight / 2);
        drawTree(scene, node->sibling, siblingX, y, xOffset, yStep,childXoffset,level);
    }
}
int getTreeDepth(Tree* node) {
    if (!node) return 0;

    int maxChildDepth = 0;
    for (int i = 0; i < 3; ++i) {
        if (node->child[i]) {
            maxChildDepth = std::max(maxChildDepth, getTreeDepth(node->child[i]));
        }
    }

    int siblingDepth = getTreeDepth(node->sibling);

    return max(maxChildDepth + 1, siblingDepth);
}
