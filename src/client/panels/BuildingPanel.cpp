#include "BuildingPanel.h"
#include "../windows/GameWindow.h"
#include <string>
#include <iostream>

BuildingPanel::BuildingPanel(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Box(cobject) {
    builder->get_widget("FactoryImage", building);
    builder->get_widget("FactoryUnitImage", unit);
    builder->get_widget("PrevUnitButton", prev);
    builder->get_widget("NextUnitButton", next);
    builder->get_widget("FactoryCreateButton", create);
    building->set("res/buildings/base_city.png");
    unit->set("res/portraits/grunt.png");
}

const std::string &BuildingPanel::get_label() {
    return label;
}

void BuildingPanel::on_click() {
    std::cout << "Hola " << std::endl;
}

Gtk::Button *BuildingPanel::next_button() {
    return next;
}

void BuildingPanel::change_unit(std::string &path) {
    unit->set(path);
}

Gtk::Button *BuildingPanel::create_button() {
    return create;
}

