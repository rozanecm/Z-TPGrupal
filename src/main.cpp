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

    int y = 8, a = 3;

    for(auto it = cell_map.begin(); it != cell_map.end(); ++it) {
        for(auto jt = it->begin(); jt != it->end(); ++jt) {
//          std::cout << jt->isEmpty() << std::endl;
            std::cout << jt->getTerrainType() << "  ";
            if (jt->getTerrainType() != "Tierra")
                std::cout << "  ";
            width += 1;
        }
        std::cout << std::endl;
        length += 1;
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    Map map(4,4,8,8, cell_map);

//    std::cout << "Probando si mapa me dice que tiene la pocision (4,27)"<< map.doesThisPositionExist(4,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-1,27)"<< map.doesThisPositionExist(-1,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-2,27)"<< map.doesThisPositionExist(-2,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,27)"<< map.doesThisPositionExist(-11,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,-27)"<< map.doesThisPositionExist(-11,-27)<<std::endl;

    Size unit_size(1,1,3,3);


    Compass compass(map, unit_size);

    Position destination(7,4);
    std::vector<Position>* road = compass.getFastestWay(unit_size.getPosition(),
                                                       destination);

    std::vector<std::vector<char>> pointed_map(9, std::vector<char>(9));
    for (int x = 0; x<= 8; ++x){
        for(int y = 0; y <= 8; ++y){
            pointed_map[x][y] = '.';
        }
    }

    for (auto x: (*road)){
        (pointed_map)[x.getX()][x.getY()] = 'x';
    }

    for (auto x: pointed_map){
        for(auto y: x){
            std::cout << y << " ";
        }
        std::cout <<std::endl;
    }

    auto app = Gtk::Application::create(argc, argv);

    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}

