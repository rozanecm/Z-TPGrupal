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

    Map map(50,50,100,100, cell_map);

//    std::cout << "Probando si mapa me dice que tiene la pocision (4,27)"<< map.doesThisPositionExist(4,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-1,27)"<< map.doesThisPositionExist(-1,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-2,27)"<< map.doesThisPositionExist(-2,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,27)"<< map.doesThisPositionExist(-11,27)<<std::endl;
//    std::cout << "Probando si mapa me dice que tiene la pocision (-11,-27)"<< map.doesThisPositionExist(-11,-27)<<std::endl;

    Size unit_size(14,14,10,10);


    Compass compass(map, unit_size);

    Position destination(89,89);
    std::vector<Position>* road = compass.getFastestWay(unit_size.getPosition(),
                                                       destination);

    std::cout << " hola1"<<std::endl;
    std::vector<std::vector<char>> pointed_map;
    for (auto x: pointed_map){
        for(auto y: x){
            y = '.';
        }
    }
    std::cout << " hola"<<std::endl;
    for (auto x: (*road)){
        (pointed_map)[x.getX()][x.getY()] = 'x';
    }

    for (auto x: pointed_map){
        for(auto y: x){
            std::cout << y;
        }
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

