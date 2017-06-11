#include "InitialWindow.h"

InitialWindow::InitialWindow(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{
    builder->get_widget("AddressEntry", address);
    builder->get_widget("PortEntry", port);
    builder->get_widget("NameEntry", name);
    builder->get_widget("ConnectButton", connect);
}
