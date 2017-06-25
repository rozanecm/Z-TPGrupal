#ifndef Z_TPGRUPAL_MAPMONITOR_H
#define Z_TPGRUPAL_MAPMONITOR_H


#include <mutex>
#include "Map.h"
#include "enums/TeamEnum.h"
#include <string>

class MapMonitor {
private:
    Map map;
    std::vector<std::string> players;

    std::mutex m;

    bool winner = false;
    bool loser = false;
public:

    void setCell(unsigned int xCoordinate,
                 unsigned int yCoordinate,
                 std::string terrainType);

    void initializeMap(unsigned int xSize, unsigned int ySize);

    unsigned int getXSize();

    unsigned int getYSize();

    std::string getTerrainTypeAt(unsigned int xCoordinate,
                                 unsigned int yCoordinate);

    void addNature(Nature nature);

    void markAsSelectedInRange(bool& terrainSelected, double xStartCoordinate,
                               double yStartCoordinate,
                               double xFinishCoordinate,
                               double yFinishCoordinate);

    std::vector<Nature>
    getNatureToDraw(unsigned int minX, unsigned int maxX, unsigned int minY,
                    unsigned int maxY);

    void update_players(const std::vector<std::string>& names);
    int get_player_id(const std::string& player);

    void finish_winner();
    void finish_loser();

    bool is_winner();
    bool is_loser();

    void update_territory(const int &id, const TeamEnum &team, const int &x,
                          const int &y);

    std::map<int, std::pair<TeamEnum, std::pair<unsigned int, unsigned int>>>
    getFlags();

    void clear();
};


#endif //Z_TPGRUPAL_MAPMONITOR_H
