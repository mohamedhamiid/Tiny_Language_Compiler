#ifndef UTILITIES_H
#define UTILITIES_H

void drawTree(QGraphicsScene* scene, Tree* node, int x, int y, int xOffset, int yStep, int childXoffset,int level);
int getTreeDepth(Tree* node);

#endif // UTILITIES_H
