#include "BuildingPanel.h"
#include "../windows/GameWindow.h"
#include <string>
#include <iostream>

#define PORTRAITS_PATH "res/portraits/"
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

    builder->get_widget("FactoryUnitHPLabel", unit_hp);
    builder->get_widget("FactoryFireRateLabel", unit_fire_rate);
    builder->get_widget("FactoryTypeLabel", unit_type);

    builder->get_widget("FactoryMinutesLabel", minutes);
    builder->get_widget("FactorySecondsLabel", seconds);
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

void BuildingPanel::set_unit_hp(int hp) {
    unit_hp->set_text(std::to_string(hp));
}

void BuildingPanel::set_unit_type(const std::string &type) {
    unit_type->set_text(type);
    unit->set(PORTRAITS_PATH + type + ".png");
}

void BuildingPanel::set_unit_fire_rate(int fire_rate) {
    unit_fire_rate->set_text(std::to_string(fire_rate));
}

Gtk::Button* BuildingPanel::prev_button() {
    return prev;
}

void BuildingPanel::set_time_left(std::pair<int, int> time) {
    if (time == std::pair<int, int>(0, 0)) {
        return;
    }
    std::string minutes = std::to_string(time.first);
    if(time.first < 10) {
        minutes = "0" + minutes;
    }
    this->minutes->set_text(minutes);

    std::string seconds = std::to_string(time.second);
    if (time.second < 10) {
        seconds = "0" + seconds;
    }
    this->seconds->set_text(seconds);
}
