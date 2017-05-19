#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "cell.h"
#include "MapLoader.h"


int main (int argc, char **argv)
{
    MapLoader map_loader("src/map.xml");
    const std::vector<std::vector<Cell>> map = map_loader.get_map();

    int length = 0, width = 0;

    int y = 8, a = 3;

    for(auto it = map.begin(); it != map.end(); ++it) {
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

    auto app = Gtk::Application::create(argc, argv);

    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}

