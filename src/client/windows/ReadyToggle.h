#ifndef Z_TPGRUPAL_READYTOGGLE_H
#define Z_TPGRUPAL_READYTOGGLE_H


#include <gtkmm/togglebutton.h>
#include <gtkmm/builder.h>

class ReadyToggle : public Gtk::ToggleButton {

public:
    ReadyToggle(BaseObjectType *cobject,
                const Glib::RefPtr<Gtk::Builder> &builder);

    void on_toggled();
};


#endif //Z_TPGRUPAL_READYTOGGLE_H
