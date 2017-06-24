#include "MapMonitor.h"
#include <Lock.h>


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

void MapMonitor::addNature(Nature nature) {
    Lock l(m);
    map.addNature(nature);
}

std::vector<Nature>
MapMonitor::getNatureToDraw(unsigned int minX, unsigned int maxX,
                            unsigned int minY, unsigned int maxY) {
    Lock l(m);
    std::vector<Nature> returnVector;

    for (Nature &nature : map.getNature()) {
        if (nature.getPosition().first >= minX and
            nature.getPosition().first <= maxX and
            nature.getPosition().second >= minY and
            nature.getPosition().second <= maxY) {
            returnVector.emplace_back(nature);
        }
    }
    return returnVector;
}
