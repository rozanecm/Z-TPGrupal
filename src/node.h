//
// Created by rodian on 15/05/17.
//

#ifndef Z_TPGRUPAL_NODE_H
#define Z_TPGRUPAL_NODE_H

#include "cell.h"
#include <iostream>
#include <vector>

// Class Node meant to be use in the calculation of A* algorithm
class Node {
private:
    int h_value, g_value;
    Node* parent;
    // This is the cell on the map that this node represents
    Cell* cell;
public:
    Node(int h_value, Cell* cell);

    void setGValue(int g, int unit_speed);

    int getFValue() const;

    int getHvalue() const;

    void setNewParent(Node* parent);

    Node* getParent() const;

    Cell* getRepresentedCell() const;

    ~Node();
};


#endif //Z_TPGRUPAL_NODE_H
