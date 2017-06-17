#include <pugixml.hpp>
#include <iostream>
#include "LoadMap.h"
#include "../enums/BuildingsEnum.h"
#include <string>
#include <vector>
#include <map>


#define ROBOT_FACTORY "res/assets/buildings/robot/base_jungle.png"
#define VEHICLE_FACTORY "res/assets/buildings/vehicle/base_jungle.png"

const std::map<std::string, BuildingsEnum> buildings {
        {std::string("VehicleFactory"), BuildingsEnum::VEHICLE_FABRIC},
        {std::string("UnitFactory"), BuildingsEnum::ROBOT_FABRIC},
        {std::string("Fort"), BuildingsEnum::FORT}
};

LoadMap::LoadMap(MapMonitor &mapMonitor, BuildingsMonitor& buildings) :
        mapMonitor(mapMonitor),
        buildingsMonitor(buildings)
{
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
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            mapMonitor.setCell(i, j, map[i][j]);
        }
    }

    pugi::xml_node territories = root.child("Territories");
    for(auto& territory : territories) {
        for (auto& factory : territory) {
            int x = std::stoi(factory.attribute("x").value());
            int y = std::stoi(factory.attribute("y").value());
            std::string name = factory.name();
            if (buildings.find(name) != buildings.end()) {
                Building b(buildings.find(name)->second, x, y);
                buildingsMonitor.addBuilding(b);
            } else {
                std::cerr << "Invalid building found: " << name << std::endl;
            }
        }

        std::string name = territory.name();
        if (buildings.find(name) != buildings.end()) {
            int x = std::stoi(territory.attribute("center_x").value());
            int y = std::stoi(territory.attribute("center_y").value());
            Building b(buildings.find(name)->second, x, y);
            buildingsMonitor.addBuilding(b);
        }
    }
}
