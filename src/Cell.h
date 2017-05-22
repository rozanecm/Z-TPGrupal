#ifndef Z_TPGRUPAL_CELL_H
#define Z_TPGRUPAL_CELL_H

#include <string>

class Cell {
private:
    std::string terrainType;

public:
    Cell();

    Cell(std::string &terrainType);

    void assignTerrainType(std::string terrainType);
};


#endif //Z_TPGRUPAL_CELL_H
