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
        // get adyacents and add them to looking list in order os F value. On tie use H value

        // check if destiny is between them
        // get the minimum F and add it to visit list (remove from looking list)
        // repeat number two.
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

std::vector<Node *> Compass::getAdyacents(Node *node) {
    std::vector<Node *> adyacents;
    // check for border situation
//    // Check left border
//    int left_pos = node->getPosition().getX() - 1;
//    if (unit_size.getWidth() <= left_pos) {
//        int y_pos = node->getPosition().getY();
//        Node* left = astar_map[left_pos][y_pos];
//        left->setGValue(SIDEWALK, map.getTerrainFactorOn(left_pos,y_pos));
//        left->setNewParent(node);
//        adyacents.push_back(left);
//    }
//
//    // Check top border
//
//    int y_min = node->getYPosition() - 1;
//
//    if (x_min < 0)
//        x_min = 0;
//
//    if (y_min < 0)
//        y_min = 0;
//
//    int x_max = node->getXPosition() + 1;
//    int y_max = node->getYPosition() + 1;
//
//    if (x_max > this->width)
//        x_max = this->width;
//
//    if (y_max > this->length)
//        y_max = this->length;
    // get the vector of adyacents
    int x_min = node->getPosition().getX() - 1;
    int x_max = node->getPosition().getX() + 1;
    int y_min = node->getPosition().getY() - 1;
    int y_max = node->getPosition().getY() + 1;

    for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
        for (int y_pos = y_min; y_pos <= y_max; ++y_pos){
            Node* ady = astar_map[x_pos][y_pos];
            Size size = ady->getSize();
            if (map.canIWalkToThisPosition(size)) {

            }
            adyacents.push_back(ady);
        }
    }
    return adyacents;
}

Compass::~Compass() {}

