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

    builder->get_widget("FactoryMaxHPLabel", max_hp_label);
    builder->get_widget("FactoryHPLabel", hp_label);
    builder->get_widget("FactoryOwnerLabel", owner_label);
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

void BuildingPanel::set_hp(int hp) {
    hp_label->set_text(std::to_string(hp));
}

void BuildingPanel::set_max_hp(int hp) {
    max_hp_label->set_text(std::to_string(hp));
}

void BuildingPanel::set_owner(const std::string &owner) {
    owner_label->set_text(owner);
}