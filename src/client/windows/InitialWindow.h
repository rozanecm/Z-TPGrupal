#ifndef Z_TPGRUPAL_INITIALWINDOW_H
#define Z_TPGRUPAL_INITIALWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <socket.h>
#include "../ServerMessenger.h"

class InitialWindow : public Gtk::Window {

    Gtk::Entry *address_entry;
    Gtk::Entry *port_entry;
    Gtk::Entry *name_entry;
    Gtk::Button *connect;
    std::shared_ptr<ServerMessenger> messenger;
    std::string name;
public:
    InitialWindow(BaseObjectType *cobject,
                  const Glib::RefPtr<Gtk::Builder> &builder);

    std::shared_ptr<ServerMessenger> get_socket();

private:

    void on_click();
};


#endif //Z_TPGRUPAL_INITIALWINDOW_H
