#include <iostream>
#include "UnitPanel.h"
#include "../enums/UnitsEnum.h"
#include "../enums/TeamEnum.h"

#define PORTRAITS "res/portraits/"
const std::map<UnitsEnum, std::string> units = {
        {UnitsEnum::GRUNT, std::string("grunt")},
        {UnitsEnum::PSYCHO, std::string("psycho"),},
        {UnitsEnum::TOUGH, std::string("tough")},
        {UnitsEnum::PYRO, std::string("pyro")},
        {UnitsEnum::SNIPER, std::string("sniper")},
        {UnitsEnum::LASER, std::string("laser")},
        {UnitsEnum::GENERIC_ROBOT, std::string("generic_robot")},
        {UnitsEnum::JEEP, std::string("jeep")},
        {UnitsEnum::MEDIUM_TANK, std::string("medium_tank")},
        {UnitsEnum::LIGHT_TANK, std::string("light_tank")},
        {UnitsEnum::HEAVY_TANK, std::string("heavy_tank")},
        {UnitsEnum::MML, std::string("mml")}
};

const std::map<TeamEnum, std::string> teams = {
        {TeamEnum::BLUE, "blue"},
        {TeamEnum::GREEN, "green"},
        {TeamEnum::RED, "red"},
        {TeamEnum ::YELLOW, "yellow"}
};

UnitPanel::UnitPanel(BaseObjectType *cobject,
                     const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Box(cobject)
{
    builder->get_widget("OwnerLabel", owner);
    builder->get_widget("MaxHPLabel", max_hp_label);
    builder->get_widget("HPLabel", hp_label);
    builder->get_widget("NameLabel", name_label);
    builder->get_widget("Portrait", portrait);

}

std::string UnitPanel::get_label() {
    return "Unit";
}

void UnitPanel::set_name(std::string name) {
    name_label->set_text(name);
}

void UnitPanel::set_hp(int hp) {
    hp_label->set_text(std::to_string(hp));
}

void UnitPanel::set_max_hp(int hp) {
    max_hp_label->set_text(std::to_string(hp));
}

void UnitPanel::set_owner(const std::string &owner) {
    this->owner->set_text(owner);
}

void UnitPanel::update_portrait(UnitsEnum unit, TeamEnum team) {
    std::string unit_name = units.find(unit)->second;
    std::string color = teams.find(team)->second;
    std::string path = PORTRAITS + unit_name + "_" + color + ".png";
    portrait->set(path);
    set_name(unit_name);
}

