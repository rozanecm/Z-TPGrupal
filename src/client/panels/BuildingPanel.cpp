#include <giomm.h>
#include "BuildingPanel.h"
#include "../windows/GameWindow.h"
#define PORTRAITS_PATH "res/portraits/"

#define TIMEOUT 100

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
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &BuildingPanel::update_labels),
                                   TIMEOUT);
}


const std::map<TeamEnum, std::string> teams = {
        {TeamEnum::NEUTRAL, "blue"},
        {TeamEnum::BLUE, "blue"},
        {TeamEnum::GREEN, "green"},
        {TeamEnum::RED, "red"},
        {TeamEnum::YELLOW, "yellow"}
};

const std::string &BuildingPanel::get_label() {
    return label;
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
    this->hp = hp;
}

void BuildingPanel::set_max_hp(int hp) {
    this->max_hp = hp;
}

void BuildingPanel::set_owner(const std::string &owner) {
    this->owner = owner;
}

void BuildingPanel::set_unit_hp(int hp) {
    unit_hp->set_text(std::to_string(hp));
}

void BuildingPanel::set_unit_type(const std::string &type, TeamEnum team) {
    std::string color = teams.find(team)->second;
    this->type = type;
    unit->set(PORTRAITS_PATH + type + "_" + color + ".png");
}

void BuildingPanel::set_unit_fire_rate(int fire_rate) {
    this->fire_rate = fire_rate;
}

Gtk::Button* BuildingPanel::prev_button() {
    return prev;
}

void BuildingPanel::set_time_left(std::pair<int, int> time) {
    this->time_left = time;
}

bool BuildingPanel::update_labels() {
    hp_label->set_text(std::to_string(hp));
    max_hp_label->set_text(std::to_string(max_hp));
    minutes->set_text(std::to_string(time_left.first));
    seconds->set_text(std::to_string(time_left.second));
    unit_fire_rate->set_text(std::to_string(fire_rate));
    unit_type->set_text(type);
    return true;
}
