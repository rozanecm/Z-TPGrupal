#ifndef Z_TPGRUPAL_INITIALWINDOW_H
#define Z_TPGRUPAL_INITIALWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>

class InitialWindow : public Gtk::Window {

    Gtk::Entry* address;
    Gtk::Entry* port;
    Gtk::Entry* name;
    Gtk::Button* connect;
public:
    InitialWindow(BaseObjectType* cobject,
        const Glib::RefPtr<Gtk::Builder>& builder);
};


#endif //Z_TPGRUPAL_INITIALWINDOW_H
