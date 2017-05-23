#include <iostream>
#include "cell.h"
#include "MapLoader.h"
#include "map.h"
#include "compass.h"
#include "unit.h"


int main (int argc, char **argv) {
    MapLoader map_loader("src/map.xml");
    std::vector<std::vector<Cell>> cell_map = map_loader.get_map();

    int length = 0, width = 0;

    Map map(7,7,15,15, cell_map);

    Size unit_size(7,4,3,3);
    Size range(-2,-2,4,4);

    Unit* unit = new Unit(100,"Grunt", 4,unit_size,range,map);
    map.addOccupant(unit);

    Size other_unit_size(1,1,3,3);
    Compass compass(map, other_unit_size, 4); // with 4 is a robot

    Position destination(13,1);
    std::vector<Position>* road = compass.getFastestWay(
                                     other_unit_size.getPosition(),destination);

    // cell map with terrain
    std::cout << std::endl;
    std::cout << std::endl;
    for(auto it = cell_map.begin(); it != cell_map.end(); ++it) {
        for(auto jt = it->begin(); jt != it->end(); ++jt) {
            std::cout << jt->getTerrainType() << "  ";
            if (jt->getTerrainType() == "Tierra")
                std::cout << "   ";
            if (jt->getTerrainType() == "Lava")
                std::cout << "     ";
            if (jt->getTerrainType() == "Agua")
                std::cout << "     ";
            width += 1;
        }
        std::cout << std::endl;
        length += 1;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::vector<std::vector<char>> pointed_map(15, std::vector<char>(15));
    for(int y = 0; y <= 14; ++y){
        for (int x = 0; x<= 14; ++x){
            pointed_map[x][y] = '.';
        }
    }

    for (auto x: (*road)){
        (pointed_map)[x.getX()][x.getY()] = 'x';
    }

    int j =0;
    for (auto x: pointed_map){
        int i = 0;

        for(auto y: x){
            ++i;
            std::cout << y << " ";
            if (3 == i) {
                std::cout << " ";
                i = 0;
            }
        }
        std::cout <<std::endl;
        ++j;
        if (3 == j) {
            std::cout <<std::endl;
            j = 0;
        }
    }
    delete(unit);
    return 0;
}

