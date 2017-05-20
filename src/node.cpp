//
// Created by rodian on 15/05/17.
//

#include "node.h"

Node::Node(int x, int y, int width, int lenght) : h_value(0), g_value(0),
                        was_visited(false), size(x, y, width, lenght) {}

void Node::setHValue(int h) {
    this->h_value = h;
}

void Node::setGValue(int g, int terrain_factor) {
    this->g_value = (g + 4)*terrain_factor;
    this->was_visited = true;
}

int Node::getGValue() {
    return this->g_value;
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

Node::~Node() {}

Position Node::getPosition() const{
    return size.getPosition();
}

Size Node::getSize() const {
    return this->size;
}

bool Node::beenSeen() const {
    return this->was_visited;
}

int Node::getFValueIfGWere(int g, int terrain_factor) {
    return (h_value + (g + 4)*terrain_factor);
}

