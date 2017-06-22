#ifndef Z_TPGRUPAL_UNITPANEL_H
#define Z_TPGRUPAL_UNITPANEL_H


#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

class UnitPanel : public Gtk::Box {
    Gtk::Label *owner;
    Gtk::Label *max_hp_label;
    Gtk::Label *hp_label;
    Gtk::Label *name_label;
public:
    UnitPanel(BaseObjectType *cobject,
              const Glib::RefPtr<Gtk::Builder> &builder);

    std::string get_label();

    void set_name(std::string name);

    void set_hp(int hp);

    void set_max_hp(int hp);

    void set_owner(const std::string& owner);
};


#endif //Z_TPGRUPAL_UNITPANEL_H
