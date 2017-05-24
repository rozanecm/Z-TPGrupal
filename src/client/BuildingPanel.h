#ifndef Z_TPGRUPAL_BUILDINGPANEL_H
#define Z_TPGRUPAL_BUILDINGPANEL_H


#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/builder.h>
#include <string>

class BuildingPanel : public Gtk::Box {
    Gtk::Image* building;
    Gtk::Image* unit;
    const std::string label = "Factory";
public:
    BuildingPanel(BaseObjectType* cobject,
                  const Glib::RefPtr<Gtk::Builder>& builder);

    const std::string& get_label();
};


#endif //Z_TPGRUPAL_BUILDINGPANEL_H
