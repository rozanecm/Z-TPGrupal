#ifndef Z_TPGRUPAL_CELL_H
#define Z_TPGRUPAL_CELL_H

#include <string>

class Cell {
private:
    std::string terrainType;

public:
    Cell();

    explicit Cell(std::string &terrainType);

    void assignTerrainType(std::string terrainType);

    std::string getTerrainType();
};


#endif //Z_TPGRUPAL_CELL_H
