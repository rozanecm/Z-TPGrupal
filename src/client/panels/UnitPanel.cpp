#include <iostream>
#include "UnitPanel.h"

UnitPanel::UnitPanel(BaseObjectType *cobject,
                     const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Box(cobject)
{
}
