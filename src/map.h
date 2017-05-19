//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_MAP_H
#define Z_TPGRUPAL_MAP_H


#include <vector>
#include "cell.h"
#include "Occupant.h"

// later written
//class Unit;
class Compass;


class Map {
private:
    std::vector<std::vector<Cell>> terrain_map;
    Size map_size;
    std::vector<Occupant> all_occupants;
//    std::vector<Unit*> all_units;

public:
    // Map receives the center position (x,y) and dimensions width and height
    Map(int x, int y, int width, int height,
        std::vector<std::vector<Cell>>& terrain_map);

    // Recieves the coordinates (x,y) and returns the terrain factor on that
    // position on the map.
    int getTerrainFactorOn(int x, int y);

    // not implemented yet
    bool areThisPointsEmpty(Size size);

    // Recieves the size of an object on the position that wants to be walk
    // Returns true if the object fits and can step to that position
    bool canIWalkToThisPosition(Size& size);

    // Returns the width of the map
    int getWidth();

    // Returns the Heigth of the map
    int getHeigth();
};


#endif //Z_TPGRUPAL_MAP_H
