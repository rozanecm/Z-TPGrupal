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
    std::vector<Position> road;
    std::map<std::string,int> terrain_modifier;
    int unit_id, unit_speed, counter;
    Size unit_size;
    bool finished, clear;
    Position destiny;
    Occupant enemy;

public:
    // The Compass receives the map of Cells for calculations and the
    // basic unit speed
    Compass(Map &map, Size &unit_size, int unit_id, int unit_speed);

    // Receives the current position of the unit and the destiny
    // Returns a vector of Cells with the fastest way
    std::vector<Position> getFastestWay(Position& from, Position& to);

    // Returns true if the position is empty
    bool canIWalkToThisPosition(Size& size);

    // Returns true if the position is empty except for the occupant
    bool canBulletWalkToThisPosition(Size& size,Occupant &shooter,
                                     Occupant& occupant);

    // Returns the Terrain factor on the (x,y) position
    double getTerrainFactorOn(int x, int y);

    void changeUnitSize(Size& unit_size);

    void changeUnitSpeed(int speed);

    void changeUnitId(int id);

    void clearCompass();

    bool checkIfItIsGrabbable(std::string& type) const;

    Occupant* checkForEnemiesOnRange(Occupant& unit, Size &range);

    // Returns the position of destiny. If destiny is not a valid position
    // it returns the closest valid position
    Position getAValidPositionForDestiny(Position& destiny);

    ~Compass();

private:
    // Builds a Node map with the size of the original map
    void buildNodeMap();

    void setTerrainModifier();
    // Writes the H value on every node of astar_map for the received position
    // It use Manhattan distance
    void setHValueForDestiny(Position& to);

    // Only valid for Manhattan distance.
    // Returns true if other is a diagonal node of reference
    // otherwise flase. Reference and other must be adjacent.
    bool isThisNodeOnDiagonal(Node* reference, Node* other);

    // Put the adyacents nodes that can be walk to on the open_nodes vector
    void getAdjacents(Node* node, int step);

    // Returns true if node and other are different nodes. Else, false
    bool isNotMe(Node* node, Node* other);

    // Puts the node in the correct order.
    // The node with lower F will be on the back
    void addToOpenInOrder(Node* node);

    // If adj node hasn't been seen or the g value from ref node is lower
    // than previous, it chages g value and the parent.
    bool writeGandSetParent(Node *ref, Node *adj, int walk, int steps);

    // Changes the position of the node
    void changeNodePosition(Node* node);

    // Inserts the node on the correct position
    void insertNodeOnOpen(Node* node);

    void getRoad(Position& from, Node* destiny);

    // Returns the closest node on the closed_nodes vector
    Node* searchForClosestNode();

    // Returns a positive value of the result of x - y.
    int getModuleOfSubtraction(int x, int y);

    // checks the Neighbor nodes to see if destiny is among them
    void checkIfIsDestinyNeighbor(Node *new_node, int step);

    // Returns the closest valid position to pos
    Position getClosestValidPosition(Position& pos);

    void addPositions(Position& position);

    void manageSteps(int& step,Position& start,
                     Position& current_pos, Position& to);

    void setHValueOnNode(Node* node);

    void addPositionsInOrder(bool increase_x, bool increase_y, int x_max,
                             int x_min, int y_max, int y_min);

    int getModule(int x, int y);
};


#endif //Z_TPGRUPAL_COMPASS_H
