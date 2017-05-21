//
// Created by rodian on 18/05/17.
//

#include "map.h"

Map::Map(int x, int y, int width, int height,
   std::vector<std::vector<Cell>>& terrain_map) : map_size(x,y,width,height),
    terrain_map(terrain_map){}

int Map::getTerrainFactorOn(int x, int y) {
    int w_cell = terrain_map[0][0].getWidthOfCell();
    int x_pos = x / w_cell;
    int y_pos = y / w_cell;

    return terrain_map[x_pos][y_pos].getMovementFactor();
}

bool Map::areThisPointsEmpty(Size size) {

    // Check on all Occupant vector if
    // this size collapse width thier own

    return true;
}

int Map::getWidth() {
    return map_size.getWidth();
}

int Map::getHeigth() {
    return map_size.getHeight();
}

bool Map::canIWalkToThisPosition(Size& other_size) {
    bool you_can = true;

    // if the object is stepping out of the map
    if (map_size.areYouHalfOutSide(other_size))
        you_can = false;
    // if the object is stepping into lava
    if (isThereLava(other_size))
        you_can = false;

    /* this->areThisPointsEmpty(other_size)*/

    return (you_can);
}

bool Map::doesThisPositionExist(int x, int y) {
    return map_size.areYouOnThisPoint(x,y);
}

bool Map::isThereLava(Size& other_size) {
    int x_max, x_min, y_max, y_min;
    other_size.calculateMaxAndMinForX(x_max, x_min);
    other_size.calculateMaxAndMinForY(y_max, y_min);

    int w_cell = terrain_map[0][0].getWidthOfCell();
    // Check if any of the corners are stepping into lava
    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            if (doesThisPositionExist(x,y)) {
                // Calculate the cell that holds this position
                int x_pos = x / w_cell;
                int y_pos = y / w_cell;

                if (terrain_map[x_pos][y_pos].getTerrainType() == "Lava") {
                    return terrain_map[x_pos][y_pos].isThereACollision(
                            other_size);
                }
            }
        }
    }

    return false;
}
