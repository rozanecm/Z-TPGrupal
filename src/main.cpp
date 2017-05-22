#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "cell.h"
#include "MapLoader.h"
#include "map.h"
#include "compass.h"


int main (int argc, char **argv) {
    MapLoader map_loader("src/map.xml");
    std::vector<std::vector<Cell>> cell_map = map_loader.get_map();

    int length = 0, width = 0;


    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    Map map(7,7,15,15, cell_map);

//    std::cout << "Probando si mapa me dice que tiene la pocision (4,27)"<< map.doesThisPositionExist(4,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-1,27)"<< map.doesThisPositionExist(-1,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-2,27)"<< map.doesThisPositionExist(-2,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,27)"<< map.doesThisPositionExist(-11,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,-27)"<< map.doesThisPositionExist(-11,-27)<<std::endl;

    Size unit_size(1,1,3,3);


    Compass compass(map, unit_size, 4); // with 4 is a robot

    Position destination(13,1);
    std::vector<Position>* road = compass.getFastestWay(unit_size.getPosition(),
                                                       destination);

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

    return 0;
}

