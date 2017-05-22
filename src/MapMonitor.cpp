#include "MapMonitor.h"
#include "Lock.h"

MapMonitor::MapMonitor(Map &map) : map(map){

}

void MapMonitor::setCell(unsigned int xCoordinate, unsigned int yCoordinate,
                         std::string terrainType) {
    Lock l(m);
    map.setCell(xCoordinate, yCoordinate, terrainType);
}

void MapMonitor::initializeMap(unsigned int xSize, unsigned int ySize) {
    Lock l(m);
    map.initializeMap(xSize, ySize);
}
