#ifndef Z_TPGRUPAL_UNITPANEL_H
#define Z_TPGRUPAL_UNITPANEL_H


#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>

class UnitPanel : public Gtk::Box {
public:
    UnitPanel(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& builder);

};


#endif //Z_TPGRUPAL_UNITPANEL_H
