#ifndef Z_TPGRUPAL_BUILDINGPANEL_H
#define Z_TPGRUPAL_BUILDINGPANEL_H


#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/builder.h>

class BuildingPanel : public Gtk::Box {

    Gtk::Image* building;
    Gtk::Image* unit;
public:
    BuildingPanel(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

};


#endif //Z_TPGRUPAL_BUILDINGPANEL_H
