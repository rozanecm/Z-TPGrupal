//
// Created by rodian on 14/05/17.
//

#ifndef Z_TPGRUPAL_COMPASS_H
#define Z_TPGRUPAL_COMPASS_H

#include <iostream>
#include <vector>
#include "cell.h"
#include "node.h"
#include "map.h"

class Map;

// class Compass so every moving unit knows the fastest way to destiny
class Compass {
    private:
    Map& map;
    std::vector<std::vector<Node*>> astar_map;
    std::vector<Node*> closed_nodes;
    std::vector<Node*> open_nodes;
    int unit_speed;
    Size unit_size;

    public:
        // The Compass receives the map of Cells for calculations and the
        // basic unit speed
        Compass(Map& map, Size unit_size);

        // Receives the current position of the unit and the destiny
        // Returns a vector of Cells with the fastest way
        std::vector<Position> getFastestWay(Position from, Position to);

        void setHValueForDestiny(Position to);

        void buildNodeMap();

        ~Compass();

    private:
        // Sets the H value for every Cell
        void setHOnYPosition(int y_dest,int x_dest, int& h_value_y);

        std::vector<Node*> getAdyacents(Node* node);
};


#endif //Z_TPGRUPAL_COMPASS_H
