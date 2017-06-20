#include "MapMonitor.h"
#include <Lock.h>

MapMonitor::MapMonitor(Map &map) : map(map) {
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

unsigned int MapMonitor::getXSize() {
    Lock l(m);
    return map.getXSize();
}

unsigned int MapMonitor::getYSize() {
    Lock l(m);
    return map.getYSize();
}

std::string MapMonitor::getTerrainTypeAt(unsigned int xCoordinate,
                                         unsigned int yCoordinate) {
    Lock l(m);
    return map.getTerrainTypeAt(xCoordinate, yCoordinate);
}

void MapMonitor::markAsSelectedInRange(bool& terrainSelected, double
                                       xStartCoordinate,
                                       double yStartCoordinate,
                                       double xFinishCoordinate,
                                       double yFinishCoordinate) {
    //todo implementar esto que falta ver el tema de accidentales sobre el mapa.
}
