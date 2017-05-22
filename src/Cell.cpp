#include "Cell.h"

Cell::Cell() : terrainType(""){

}

Cell::Cell(std::string &terrainType) : terrainType(terrainType) {

}

void Cell::assignTerrainType(std::string terrainType) {
    this->terrainType = terrainType;
}
