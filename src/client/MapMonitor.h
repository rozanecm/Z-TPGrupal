#ifndef Z_TPGRUPAL_MAPMONITOR_H
#define Z_TPGRUPAL_MAPMONITOR_H


#include <mutex>
#include "Map.h"
#include <string>

class MapMonitor {
private:
    Map &map;

    std::mutex m;

public:
    explicit MapMonitor(Map &map);

    void setCell(unsigned int xCoordinate,
                 unsigned int yCoordinate,
                 std::string terrainType);

    void initializeMap(unsigned int xSize, unsigned int ySize);

    unsigned int getXSize();

    unsigned int getYSize();

    std::string getTerrainTypeAt(unsigned int xCoordinate,
                                 unsigned int yCoordinate);

    void markAsSelectedInRange(double xStartCoordinate, double yStartCoordinate,
                               double xFinishCoordinate, double yFinishCoordinate);
};


#endif //Z_TPGRUPAL_MAPMONITOR_H
