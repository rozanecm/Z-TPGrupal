#include "ReadyToggle.h"

ReadyToggle::ReadyToggle(BaseObjectType *cobject,
                         const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::ToggleButton(cobject)
{

}

void ReadyToggle::on_toggled() {
    return Gtk::ToggleButton::on_toggled();
}
