#include <gtkmm/builder.h>
#include <giomm.h>
#include <gdkmm.h>
#include "GameArea.h"

GameArea::GameArea() {

}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("res/portraits/sample.png");
    // Draw the image at 110, 90, except for the outermost 10 pixels.
    Gdk::Cairo::set_source_pixbuf(cr, image, 100, 80);
    cr->rectangle(110, 90, image->get_width()-20, image->get_height()-20);
    cr->fill();
    return true;
}
