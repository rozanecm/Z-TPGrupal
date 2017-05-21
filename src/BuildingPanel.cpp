#include "BuildingPanel.h"

BuildingPanel::BuildingPanel(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Box(cobject)
{
    builder->get_widget("FactoryImage", building);
    builder->get_widget("FactoryUnitImage", unit);

    building->set("res/buildings/base_city.png");
    unit->set("res/portraits/sample.png");
}
