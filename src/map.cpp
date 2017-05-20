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

    return (!map_size.areYouHalfOutSide(other_size) ||
            !isThereLava(other_size)/* &&
            this->areThisPointsEmpty(other_size)*/);
}

bool Map::doesThisPositionExist(int x, int y) {
    return map_size.areYouOnThisPoint(x,y);
}

bool Map::isThereLava(Size& other_size) {
    int w_cell = terrain_map[0][0].getWidthOfCell();
    int x_pos = other_size.getPosition().getX() / w_cell;
    int y_pos = other_size.getPosition().getY() / w_cell;

    if (terrain_map[x_pos][y_pos].getTerrainType() == "Lava"){
      return  terrain_map[x_pos][y_pos].isThereACollision(other_size);
    }

    return false;
}
