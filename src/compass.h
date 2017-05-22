//
// Created by rodian on 14/05/17.
//

#ifndef Z_TPGRUPAL_COMPASS_H
#define Z_TPGRUPAL_COMPASS_H

#include <iostream>
#include <vector>
#include <map>
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
    std::vector<Position>* road;
    std::map<std::string,int> terrain_modifier;
    int unit_speed;
    Size unit_size;

public:
    // The Compass receives the map of Cells for calculations and the
    // basic unit speed
    Compass(Map& map, Size unit_size, int unit_speed);

    // Receives the current position of the unit and the destiny
    // Returns a vector of Cells with the fastest way
    std::vector<Position>* getFastestWay(Position from, Position to);

    void buildNodeMap();

    ~Compass();

private:
    void setTerrainModifier(int unit_speed);
    // Writes the H value on every node of astar_map for the received position
    // It use Manhattan distance
    void setHValueForDestiny(Position to);

    // Only valid for Manhattan distance.
    // Returns true if other is a diagonal node of reference
    // otherwise flase. Reference and other must be adjacent.
    bool isThisNodeOnDiagonal(Node* reference, Node* other);

    // Put the adyacents nodes that can be walk to on the open_nodes vector
    void getAdjacents(Node* node);

    // Returns true if node and other are different nodes. Else, false
    bool isNotMe(Node* node, Node* other);

    // Puts the node in the correct order.
    // The node with lower F will be on the back
    void addToOpenInOrder(Node* node);

    // If adj node hasn't been seen or the g value from ref node is lower
    // than previous, it chages g value and the parent.
    bool writeGandSetParent(Node* ref, Node* adj, int walk);

    // Changes the position of the node
    void changeNodePosition(Node* node);

    // Inserts the node on the correct position
    void insertNodeOnOpen(Node* node);

    void getRoad(Position from, Node* destiny);

    Node* searchForClosestNode();

    // Returns a positive value of the result of x - y.
    int getModuleOfSubtraction(int x, int y);

    void checkIfIsDestinyNeighbor(Node* new_node);
};


#endif //Z_TPGRUPAL_COMPASS_H
