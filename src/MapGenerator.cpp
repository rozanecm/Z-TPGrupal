#include <fstream>
#include "MapGenerator.h"
#include "pugixml.hpp"

using namespace pugi;
MapGenerator::MapGenerator(int size) :
    size(size)
{
}

void MapGenerator::generate(const std::string& name) {
    std::string path = "maps/" + name + ".xml";
    xml_document document;
    xml_node root = document.append_child("Map");
    for (int i = 0; i < size; ++i) {
        xml_node row = root.append_child("Row");
        for (int j = 0; j < size; ++j) {
            xml_node cell = row.append_child("Cell");
            xml_attribute attr = cell.append_attribute("terrain");
            attr.set_value("Tierra");
        }
    }
    document.save_file(path.c_str());
}

MapGenerator::~MapGenerator() {
    if (output.is_open()) {
        output.close();
    }
}



