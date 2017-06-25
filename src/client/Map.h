#ifndef Z_TPGRUPAL_MAP_H
#define Z_TPGRUPAL_MAP_H


#include <vector>
#include "Cell.h"
#include "Nature.h"
#include "enums/TeamEnum.h"
#include <string>
#include <map>

class Map {
private:
    std::vector<std::vector<Cell>> baseMap;
    std::vector<Nature> nature;
    /**vector storing flags' positions.
    * Key = ID,
    * Value = pair team, coordinates
    */
    std::map<int, std::pair<TeamEnum, std::pair<unsigned int, unsigned int>>>
            flags;

public:
    void initializeMap(unsigned int xSize, unsigned int ySize);

    void addNature(Nature nature);

    /* vector storing all nature, i.e. rocks, and all which is not plain
     * terrain or buildings and independent units, in the map. */
    std::vector<Nature> getNature();

    void setCell(unsigned int xCoordinate,
                 unsigned int yCoordinate,
                 std::string terrainType);

    unsigned int getXSize();

    unsigned int getYSize();

    std::string getTerrainTypeAt(unsigned int xCoordinate,
                                 unsigned int yCoordinate);

    void update_territory(const int &id, const TeamEnum &team, const int &x,
                          const int &y);

    std::map<int, std::pair<TeamEnum, std::pair<unsigned int, unsigned int>>>
    getFlags();

    void clear();
};


#endif //Z_TPGRUPAL_MAP_H
