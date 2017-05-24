//
// Created by rodian on 15/05/17.
//

#ifndef Z_TPGRUPAL_NODE_H
#define Z_TPGRUPAL_NODE_H

#include "cell.h"
#include "size.h"
#include <iostream>
#include <vector>

// Class Node meant to be use in the calculation of A* algorithm
class Node {
private:
    int h_value, g_value;
    Node* parent;
    bool was_visited;
    Size size;

public:
    Node(int x, int y, int width, int lenght);

    void setHValue(int h);

    void setGValue(int g, int terrain_factor);

    int getGValue();

    int getFValue() const;

    int getHvalue() const;

    void setNewParent(Node* parent);

    Node* getParent() const;

    Position getPosition() const;

    Size getSize() const;

    bool beenSeen() const;

    int getFValueIfGWere(int g, int terrain_factor);

    ~Node();
};

#endif //Z_TPGRUPAL_NODE_H
