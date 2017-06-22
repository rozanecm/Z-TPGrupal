#include <iostream>
#include "UnitPanel.h"

UnitPanel::UnitPanel(BaseObjectType *cobject,
                     const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Box(cobject)
{
    builder->get_widget("OwnerLabel", owner);
    builder->get_widget("MaxHPLabel", max_hp_label);
    builder->get_widget("HPLabel", hp_label);
    builder->get_widget("NameLabel", name_label);
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

