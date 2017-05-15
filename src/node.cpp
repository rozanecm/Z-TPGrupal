//
// Created by rodian on 15/05/17.
//

#include "node.h"

Node::Node(int h, Cell* cell) : h_value(h), g_value(0), cell(cell) {}

void Node::setGValue(int g, int unit_speed) {
    this->g_value = g + (unit_speed *cell->getMovementFactor());
}

int Node::getFValue() const {
    return (h_value + g_value);
}

int Node::getHvalue() const {
    return this->h_value;
}

void Node::setNewParent(Node *parent) {
    this->parent = parent;
}

Node *Node::getParent() const {
    return this->parent;
}
Cell *Node::getRepresentedCell() const {
    return this->cell;
}

Node::~Node() {}
