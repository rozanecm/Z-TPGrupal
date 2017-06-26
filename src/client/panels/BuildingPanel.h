#ifndef Z_TPGRUPAL_BUILDINGPANEL_H
#define Z_TPGRUPAL_BUILDINGPANEL_H


#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/builder.h>
#include <string>
#include <gtkmm/button.h>
#include "../enums/TeamEnum.h"

class BuildingPanel : public Gtk::Box {
    Gtk::Image* building;
    Gtk::Image* unit;
    const std::string label = "Factory";
    Gtk::Button* prev;
    Gtk::Button* next;
    Gtk::Button* create;

    Gtk::Label* max_hp_label;
    Gtk::Label* hp_label;
    Gtk::Label* owner_label;
    Gtk::Label* unit_type;
    Gtk::Label* unit_hp;
    Gtk::Label* unit_fire_rate;

    Gtk::Label* minutes;
    Gtk::Label* seconds;

    int max_hp;
    int hp;
    std::string owner;
    std::string type;
    int fire_rate;
    std::pair<int, int> time_left;
public:
    BuildingPanel(BaseObjectType* cobject,
                  const Glib::RefPtr<Gtk::Builder>& builder);

    const std::string& get_label();
    Gtk::Button* next_button();
    Gtk::Button* create_button();

    void change_unit(std::string& path);


    void set_hp(int hp);

    void set_max_hp(int hp);

    void set_owner(const std::string& owner);

    void set_unit_hp(int hp);

    void set_unit_type(const std::string& type, TeamEnum team);

    void set_unit_fire_rate(int fire_rate);

    void set_time_left(std::pair<int, int> time);
    Gtk::Button * prev_button();

protected:
    bool update_labels();
};


#endif //Z_TPGRUPAL_BUILDINGPANEL_H
