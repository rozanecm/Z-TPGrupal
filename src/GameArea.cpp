#include <gtkmm/builder.h>
#include <giomm.h>
#include <gdkmm.h>
#include "GameArea.h"

GameArea::GameArea() {

}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    // Draw the image at 110, 90, except for the outermost 10 pixels.
    Gdk::Cairo::set_source_pixbuf(cr, image, 100, 80);
    cr->rectangle(110, 90, image->get_width()-20, image->get_height()-20);
    cr->fill();
    return true;
}
