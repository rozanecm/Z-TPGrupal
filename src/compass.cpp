//
// Created by rodian on 14/05/17.
//

#include "compass.h"

#define SIDEWALK 10
#define DIAGONALWALK 14

Compass::Compass(Map &map, Size unit_size): map(map), unit_size(unit_size) {
    this->buildNodeMap();
}

void Compass::buildNodeMap() {
    // the nodes has the size of the unit that is using this compass
    for(int it = 0; it <= map.getWidth(); ++it) {
        std::vector<Node*> row_vec;
        astar_map.push_back(row_vec);
        for(int jt = 0; jt <= map.getHeigth(); ++jt) {
            astar_map.back().push_back(new Node(it, jt,
                    unit_size.getWidth(), unit_size.getHeight()));
        }
    }
}

std::vector<Position> Compass::getFastestWay(Position from, Position to) {
    std::vector<Position> road;
    // set H value for destiny
    this->setHValueForDestiny(to);
    // start algorithm
        // add "from" to visited list
    Node* start_node = astar_map[from.getX()][from.getY()];
    start_node->setGValue(0,map.getTerrainFactorOn(from.getX(),from.getY()));
    this->closed_nodes.push_back(start_node);

    Node *closer_node = start_node;
    // While haven't reach destiny node or open_nodes has nodes to visit.
    bool finished = false;
    while (!finished && (!open_nodes.empty())) {
        // get adyacents and add them to looking list in order of F value.
        // On tie use H value.
        this->getAdyacents(closer_node);

        // get the minimum F and add it to visit list (remove from looking list)
        closer_node = open_nodes.back();
        open_nodes.pop_back();
        this->closed_nodes.push_back(closer_node);

        // check if destiny is between them
        if (closed_nodes.back()->getHvalue() == 0)
            finished = true;
    }
    Node* closest;
    if (finished) {
        road = this->getRoad(closer_node);
    } else {
        closest = this->searchForClosestNode();
        road = this->getRoad(closest);
    }
    return road;
}

void Compass::setHValueForDestiny(Position to) {
    astar_map[to.getX()][to.getY()]->setHValue(0);
    // Set counter to 0
    int h_value_x = 0, h_value_y = 0;

    // For every value of 'x' til max width
    for(int x_pos = to.getX(); x_pos <= map.getWidth() ; ++x_pos) {
        this->setHOnYPosition(x_pos, to.getY(), h_value_y);
        h_value_y = ++h_value_x;
    }

    // For every value of 'x' til min width
    h_value_x = 0, h_value_y = 0;
    for(int x_pos = to.getX(); x_pos >= 0; --x_pos) {
        this->setHOnYPosition(x_pos, to.getY(), h_value_y);
        h_value_y = ++h_value_x;
    }
}

void Compass::setHOnYPosition(int x_pos, int y_dest, int& h_value_y) {
    // For every value of 'y' til max length
    for(int y_pos = y_dest; y_pos <= map.getHeigth(); ++y_pos) {
        astar_map[x_pos][y_pos]->setHValue(h_value_y);
        ++h_value_y;
    }
    // For every value of 'y' til min length
    h_value_y = 0;
    for(int y_pos = y_dest; y_pos >= 0; --y_pos) {
        astar_map[x_pos][y_pos]->setHValue(h_value_y);
        ++h_value_y;
    }
}

void Compass::getAdyacents(Node *node) {
    // get limits
    int x_min = node->getPosition().getX() - 1;
    int x_max = node->getPosition().getX() + 1;
    int y_min = node->getPosition().getY() - 1;
    int y_max = node->getPosition().getY() + 1;

    bool adj_new_g;
    for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
        for (int y_pos = y_min; y_pos <= y_max; ++y_pos){
            Node* adj = astar_map[x_pos][y_pos];
            Size size = adj->getSize();

            // Check if whether node fit or the position is not available.
            // Also discard the node looking for his adjacent
            if ((map.canIWalkToThisPosition(size)) && this->isNotMe(node,adj)) {

                // G value differs when the node is diagonal or next to it
                if (this->isThisNodeOnDiagonal(node, adj)) {
                    adj_new_g = this->writeGandSetParent(node,adj,DIAGONALWALK);

                } else {
                    adj_new_g = this->writeGandSetParent(node,adj,SIDEWALK);
                }
                if (adj_new_g)
                    this->addToOpenInOrder(adj);
            }
        }
    }
}

Compass::~Compass() {}

bool Compass::isThisNodeOnDiagonal(Node* ref, Node* other) {
    int diff = ref->getHvalue() - other->getHvalue();
    return (diff > 1) || (diff < -1);
}

bool Compass::isNotMe(Node *node, Node* other) {
    Position ref = node->getPosition();
    Position ady = other->getPosition();
    return !((ref.getX() == ady.getX()) && (ref.getY() == ady.getY()));
}

void Compass::addToOpenInOrder(Node *new_node) {
    // Only add to the vector those that haven't been seen
    if (!new_node->beenSeen()){
        this->insertNodeOnOpen(new_node);
    } else {
        this->changeNodePosition(new_node);
    }
}

bool Compass::writeGandSetParent(Node *ref, Node *adj, int walk) {
    int new_g = walk + ref->getGValue();
    bool adj_change_g = false;
    // if g value from node is lower than previous or this
    // adjacent hasn't been seen yet,
    // add new g value and change parent.
    if ((adj->beenSeen() && new_g < adj->getGValue()) ||
        (!adj->beenSeen())) {
        Position pos = adj->getPosition();
        adj->setGValue(new_g, map.getTerrainFactorOn(pos.getX(),pos.getY()));
        adj->setNewParent(ref);
        adj_change_g = true;
    }
    return adj_change_g;
}

void Compass::changeNodePosition(Node *node) {
    // first erase node from vector
    bool erased = false;
    Position node_pos = node->getPosition();
    std::vector<Node *>::iterator it = open_nodes.begin();
    while ((!erased) && (it != open_nodes.end())) {
        Position it_pos = (*it)->getPosition();
        if ((it_pos.getX() == node_pos.getX()) &&
                (it_pos.getY() == node_pos.getY())){
            it = open_nodes.erase(it);
            erased = true;
        } else {
            ++it;
        }
    }
    // Add it again in correct position
    this->insertNodeOnOpen(node);
}

void Compass::insertNodeOnOpen(Node *new_node) {
    bool inserted = false;

    // Save nodes by F value. The lowest on the back.
    // If two nodes have same F value, the one with the lowest H value
    // will be closer to the back.
    std::vector<Node *>::iterator it = open_nodes.begin();
    while ((!inserted) && (it != open_nodes.end())) {
        if (((*it)->getFValue()) < new_node->getFValue()) {
            open_nodes.insert(it, new_node);
            inserted = true;
        } else if (((*it)->getFValue()) == new_node->getFValue()) {
            if (((*it)->getHvalue()) < new_node->getHvalue()) {
                open_nodes.insert(it, new_node);
                inserted = true;
            }
        }
        ++it;
    }
    if (!inserted) {
        open_nodes.push_back(new_node);
    }
}

std::vector<Position> Compass::getRoad(Node *destiny) {
    std::vector<Position> road;
    road.push_back(destiny->getPosition());
    Node* next_node = destiny->getParent();
    while (next_node->getGValue() != 0) {
        road.push_back(next_node->getPosition());
    }
    return road;
}

Node *Compass::searchForClosestNode() {
    Node* closest = closed_nodes.front();
    for (auto x: closed_nodes){
        if ((x->getHvalue() < closest->getHvalue()) ||
         ((x->getHvalue() == closest->getGValue()) &&
          (x->getFValue() < closest->getFValue()))) {
            closest = x;
        }
    }
    return closest;
}

