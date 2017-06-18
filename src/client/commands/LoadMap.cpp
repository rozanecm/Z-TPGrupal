#include <pugixml.hpp>
#include <iostream>
#include "LoadMap.h"


#define ROBOT_FACTORY "res/assets/buildings/robot/base_jungle.png"
#define VEHICLE_FACTORY "res/assets/buildings/vehicle/base_jungle.png"

const std::map<std::string, BuildingsEnum> buildings{
        {std::string("VehicleFactory"), BuildingsEnum::VEHICLE_FABRIC},
        {std::string("UnitFactory"),    BuildingsEnum::ROBOT_FABRIC},
        {std::string("Fort"),           BuildingsEnum::FORT}
};

LoadMap::LoadMap(MapMonitor &mapMonitor, BuildingsMonitor &buildings,
                 GameWindow &window) :
        mapMonitor(mapMonitor), buildingsMonitor(buildings), window(window) {
}

void LoadMap::execute(const std::vector<std::string> &args) {
    /* initialize map so then can be completed with read data */
    mapMonitor.initializeMap(100, 100);

    std::vector<std::vector<std::__cxx11::string>> map;
    pugi::xml_document doc;
    /* the only arg we receive is the map,
     * which is the whole xml saved in a string */
    pugi::xml_parse_result result = doc.load_string(args[0].c_str());
    if (!result) {
        std::cerr << "FATAL ERROR LOADING MAP: " << result.description();
        return;
    }

    pugi::xml_node root = doc.child("Map");
    pugi::xml_node terrain = root.child("Terrain");
    for (auto node_row : terrain.children()) {
        std::vector<std::__cxx11::string> row;
        for (auto cell : node_row.children()) {
            row.push_back(cell.attribute("terrain").value());
        }
        map.push_back(row);
    }

    unsigned long size = map.size();
    mapMonitor.initializeMap(size, size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            mapMonitor.setCell(i, j, map[i][j]);
        }
    }
    window.setMapData();
}
