#include <gtkmm/builder.h>
#include <utility>
#include <gdkmm.h>
#include <iostream>
#include "GameArea.h"
#include <string>


#define TILESIZE 16    //tile width in pixels
#define NUMBER_OF_TILES_TO_SHOW 10

GameArea::GameArea(BaseObjectType *cobject,
                   const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::DrawingArea(cobject),
        flagCounter(0),
        jeepCounter(0),
        tireCounter(0),
        lightTankCounter(0),
        mediumTankCounter(0),
        heavyTankCounter(0),
        mmlCounter(0),
        playersMonitor(nullptr),
        buildingsMonitor(nullptr),
        mapMonitor(nullptr),
        /* camera is initialized with size 0,0 because we dont
         * have this data yet */
        camera(TILESIZE, 0, 0, NUMBER_OF_TILES_TO_SHOW) {
    /* load flags animations */
    loadFlagAnimations();
    /* load units resources */
    loadUnitsResources();
    /* load buildings resources */
    loadBuildingsResources();

    /* load some img */
    someImg = Gdk::Pixbuf::create_from_file("res/portraits/grunt.png");

    /* Load tiles */
    tiles["Tierra"] = Gdk::Pixbuf::create_from_file
            ("res/assets/tiles/tierra.png");
    tiles["Agua"] = Gdk::Pixbuf::create_from_file
            ("res/assets/tiles/agua.png");
    tiles["Lava"] = Gdk::Pixbuf::create_from_file("res/assets/tiles/lava.png");

    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::BUTTON_RELEASE_MASK);
    add_events(Gdk::EventMask::KEY_PRESS_MASK);
    set_can_focus(true);
}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    if (selectionMade)
        processSelection();
    drawBaseMap(cr, camera.getPosition());
    drawFlagAnimation(cr, 500, 500);
    return true;
}

void GameArea::drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr,
                           std::pair<unsigned int, unsigned int>
                           cameraPosition) {
    /* cameraPosition is given in pixels.
     * i,j indicate TILES. */
    for (unsigned int i = 0; i < NUMBER_OF_TILES_TO_SHOW; ++i){
        for (unsigned int j = 0; j < NUMBER_OF_TILES_TO_SHOW; ++j){
            drawTileAt(cr, i, j, mapMonitor->getTerrainTypeAt(
                    cameraPosition.first/TILESIZE-NUMBER_OF_TILES_TO_SHOW/2 + i,
                    cameraPosition.second/TILESIZE-NUMBER_OF_TILES_TO_SHOW/2 +
                            j));
        }
    }
}

void GameArea::drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                          unsigned int xCoordinate, unsigned int yCoordinate,
                          std::string terrainType) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, tiles.at(terrainType),
                                  xCoordinate*get_width()/
                                  NUMBER_OF_TILES_TO_SHOW,
                                  yCoordinate*get_height()/
                                  NUMBER_OF_TILES_TO_SHOW);

    cr->rectangle(xCoordinate * get_width()/NUMBER_OF_TILES_TO_SHOW,
                  yCoordinate * get_height()/NUMBER_OF_TILES_TO_SHOW,
                  get_width()/NUMBER_OF_TILES_TO_SHOW,
                  get_height()/NUMBER_OF_TILES_TO_SHOW);
    cr->fill();
    cr->restore();
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr,
                                 int xCoordinate, int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, flags.at(FlagEnum::BLUE).at(flagCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  flags.at(FlagEnum::BLUE).at(flagCounter)->get_width(),
                  flags.at(FlagEnum::BLUE).at(flagCounter)->get_height());
    cr->fill();
    flagCounter == flags.at(FlagEnum::BLUE).size()-1 ? (flagCounter = 0)
                                                     : (flagCounter++);
    cr->restore();
}

void
GameArea::displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &cr,
                               int xCoordinate,
                               int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, someImg, xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  someImg->get_width(), someImg->get_height());
    cr->fill();
    cr->restore();
}

void GameArea::setResources(PlayersMonitor *playersMonitor,
                            BuildingsMonitor *buildingsMonitor,
                            MapMonitor *mapMonitor) {
    this->playersMonitor = playersMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
    this->camera.setMapWidth(mapMonitor->getXSize());
    this->camera.setMapHeight(mapMonitor->getYSize());
}

bool GameArea::on_key_press_event(GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Up and event->keyval == GDK_KEY_Left) {
        camera.moveUp();
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Down){
        camera.moveDown();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Left){
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Right){
        camera.moveRight();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Up) {
        camera.moveUp();
        //returning true, cancels the propagation of the event
        return true;
    }
//  todo ver si el event handling se pasa arriba o no
//    //if the event has not been handled, call the base class
//    return Gtk::Window::on_key_press_event(key_event);
}

bool GameArea::on_button_press_event(GdkEventButton *event) {
    /** From https://developer.gnome.org/gdk3/stable/gdk3-Event
     *                          -Structures.html#GdkEventButton
     *
     * GdkEventType type;   the type of the event (GDK_BUTTON_PRESS,
     *                      GDK_2BUTTON_PRESS, GDK_3BUTTON_PRESS or
     *                      GDK_BUTTON_RELEASE).
     *
     * GdkWindow *window;   the window which received the event.
     *
     * gint8 send_event;    TRUE if the event was sent explicitly.
     *
     * guint32 time;        the time of the event in milliseconds.
     *
     * gdouble x;           the x coordinate of the pointer relative to the
     *                      window.
     *
     * gdouble y;           the y coordinate of the pointer relative to the
     *                      window.
     *
     * gdouble *axes;       x , y translated to the axes of device , or NULL
     *                      if device is the mouse.
     *
     * guint state;         a bit-mask representing the state of the modifier
     *                      keys (e.g. Control, Shift and Alt) and the pointer
     *                      buttons. See GdkModifierType.
     *
     * guint button;        the button which was pressed or released, numbered
     *                      from 1 to 5. Normally button 1 is the left mouse
     *                      button, 2 is the middle button, and 3 is the right
     *                      button. On 2-button mice, the middle button can
     *                      often be simulated by pressing both
     *                      mouse buttons together.
     *
     * GdkDevice *device;   the master device that the event originated from.
     *                      Use gdk_event_get_source_device()
     *                      to get the slave device.
     *
     * gdouble x_root;      the x coordinate of the pointer relative to
     *                      the root of the screen.
     *
     * gdouble y_root;      the y coordinate of the pointer relative to
     *                      the root of the screen.
     *
     */
    if (event->button == 1) {
        xStartCoordinate = event->x;
        yStartCoordinate = event->y;
        /* returning true, cancels the propagation of the event */
        return true;
    }
}

bool GameArea::on_button_release_event(GdkEventButton *event) {
    if (event->button == 1) {
        xFinishCoordinate = event->x;
        yFinishCoordinate = event->y;
        selectionMade = true;
        //returning true, cancels the propagation of the event
        return true;
    }
}

void GameArea::processSelection() {
    //todo processing logic
    /* tell each of the structures storing objects in the map to mark as
     * selected the items which are within the mouse selection */
    playersMonitor->markAsSelectedInRange(
            xStartCoordinate + camera.cameraOffset().first,
            yStartCoordinate + camera.cameraOffset().second,
            xFinishCoordinate + camera.cameraOffset().first,
            yFinishCoordinate + camera.cameraOffset().second);
    buildingsMonitor->markAsSelectedInRange(
            xStartCoordinate + camera.cameraOffset().first,
            yStartCoordinate + camera.cameraOffset().second,
            xFinishCoordinate + camera.cameraOffset().first,
            yFinishCoordinate + camera.cameraOffset().second);
    mapMonitor->markAsSelectedInRange(
            xStartCoordinate + camera.cameraOffset().first,
            yStartCoordinate + camera.cameraOffset().second,
            xFinishCoordinate + camera.cameraOffset().first,
            yFinishCoordinate + camera.cameraOffset().second);
    selectionMade = false;
}

void GameArea::loadUnitsResources() {
    /* load fire animations */
    loadGruntFireAnimations();
    loadLaserFireAnimations();
    loadPsychoFireAnimations();
    loadPyroFireAnimations();
    loadSniperFireAnimations();
    loadToughFireAnimations();

    /* load walking animations */
    loadBlueWalkingAnimations();
    loadGreenWalkingAnimations();
    loadRedWalkingAnimations();
    loadYellowWalkingAnimations();

    /* load standing animations */
    loadBlueStandingAnimations();
    loadGreenStandingAnimations();
    loadRedStandingAnimations();
    loadYellowStandingAnimations();

    /* load vehicles' animations */
    loadTiresAnimations();
    loadNeuterVehiclesAnimations();
    loadBlueVehiclesAnimations();
    loadGreenVehiclesAnimations();
    loadRedVehiclesAnimations();
    loadYellowVehiclesAnimations();
}

/* FLAGS animations loading */
void GameArea::loadFlagAnimations() {
    /* this methods loads all the imgs needed to draw all the flags' animations.
     * POSSIBLE OPTIMIZATION: load only needed colors */
    flags[FlagEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n00.png"));

    flags[FlagEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n01.png"));

    flags[FlagEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n02.png"));

    flags[FlagEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n03.png"));

    /* load red flags imgs */
    flags[FlagEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n00.png"));

    flags[FlagEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n01.png"));

    flags[FlagEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n02.png"));

    flags[FlagEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n03.png"));

    /* load yellow flags imgs */
    flags[FlagEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n00.png"));

    flags[FlagEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n01.png"));

    flags[FlagEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n02.png"));

    flags[FlagEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n03.png"));

    /* load green flags imgs */
    flags[FlagEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n00.png"));

    flags[FlagEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n01.png"));

    flags[FlagEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n02.png"));

    flags[FlagEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n03.png"));

    /* load neuter flags imgs */
    flags[FlagEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n00.png"));

    flags[FlagEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n01.png"));

    flags[FlagEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n02.png"));

    flags[FlagEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n03.png"));
}

/* FIRE animations loading */
void GameArea::loadGruntFireAnimations()  {
    loadBlueGruntFireAnimations();
    loadGreenGruntFireAnimations();
    loadRedGruntFireAnimations();
    loadYellowGruntFireAnimations();
}

void GameArea::loadBlueGruntFireAnimations() {
    /* blue grunt fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n04.png"));

    /* blue grunt fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n04.png"));

    /* blue grunt fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n04.png"));

    /* blue grunt fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n04.png"));

    /* blue grunt fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n04.png"));

    /* blue grunt fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n04.png"));

    /* blue grunt fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n04.png"));

    /* blue grunt fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenGruntFireAnimations() {
    /* green grunt fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n04.png"));

    /* green grunt fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n04.png"));

    /* green grunt fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n04.png"));

    /* green grunt fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n04.png"));

    /* green grunt fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n04.png"));

    /* green grunt fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n04.png"));

    /* green grunt fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n04.png"));

    /* green grunt fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n04.png"));
}

void GameArea::loadRedGruntFireAnimations() {
    /* red grunt fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n04.png"));

    /* red grunt fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n04.png"));

    /* red grunt fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n04.png"));

    /* red grunt fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n04.png"));

    /* red grunt fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r180_n04.png"));

    /* red grunt fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r225_n04.png"));

    /* red grunt fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r270_n04.png"));

    /* red grunt fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r315_n04.png"));
}

void GameArea::loadYellowGruntFireAnimations() {
    /* yellow grunt fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r000_n04.png"));

    /* yellow grunt fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r045_n04.png"));

    /* yellow grunt fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r090_n04.png"));

    /* yellow grunt fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r135_n04.png"));

    /* yellow grunt fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r180_n04.png"));

    /* yellow grunt fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r225_n04.png"));

    /* yellow grunt fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r270_n04.png"));

    /* yellow grunt fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_yellow_r315_n04.png"));
}

void GameArea::loadLaserFireAnimations() {
    loadBlueLaserFireAnimations();
    loadGreenLaserFireAnimations();
    loadRedLaserFireAnimations();
    loadYellowLaserFireAnimations();
}

void GameArea::loadBlueLaserFireAnimations() {
    /* blue laser fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r000_n02.png"));

    /* blue laser fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r045_n02.png"));

    /* blue laser fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r090_n02.png"));

    /* blue laser fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r135_n02.png"));

    /* blue laser fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r180_n02.png"));

    /* blue laser fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r225_n02.png"));

    /* blue laser fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r270_n02.png"));

    /* blue laser fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenLaserFireAnimations() {
    /* green laser fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r000_n02.png"));

    /* green laser fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r045_n02.png"));

    /* green laser fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r090_n02.png"));

    /* green laser fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r135_n02.png"));

    /* green laser fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r180_n02.png"));

    /* green laser fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r225_n02.png"));

    /* green laser fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r270_n02.png"));

    /* green laser fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_green_r315_n02.png"));
}

void GameArea::loadRedLaserFireAnimations() {
    /* red laser fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r000_n02.png"));

    /* red laser fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r045_n02.png"));

    /* red laser fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r090_n02.png"));

    /* red laser fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r135_n02.png"));

    /* red laser fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r180_n02.png"));

    /* red laser fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r225_n02.png"));

    /* red laser fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r270_n02.png"));

    /* red laser fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_red_r315_n02.png"));
}

void GameArea::loadYellowLaserFireAnimations() {
    /* yellow laser fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r000_n02.png"));

    /* yellow laser fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r045_n02.png"));

    /* yellow laser fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r090_n02.png"));

    /* yellow laser fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r135_n02.png"));

    /* yellow laser fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r180_n02.png"));

    /* yellow laser fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r225_n02.png"));

    /* yellow laser fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r270_n02.png"));

    /* yellow laser fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/laser/fire_yellow_r315_n02.png"));
}

void GameArea::loadPsychoFireAnimations() {
    loadBluePsychoFireAnimations();
    loadGreenPsychoFireAnimations();
    loadRedPsychoFireAnimations();
    loadYellowPsychoFireAnimations();
}

void GameArea::loadBluePsychoFireAnimations() {
    /* blue psycho fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r000_n01.png"));

    /* blue psycho fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r045_n01.png"));

    /* blue psycho fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r090_n01.png"));

    /* blue psycho fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r135_n01.png"));

    /* blue psycho fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r180_n01.png"));

    /* blue psycho fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r225_n01.png"));

    /* blue psycho fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r270_n01.png"));

    /* blue psycho fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_blue_r315_n01.png"));
}

void GameArea::loadGreenPsychoFireAnimations() {
    /* green psycho fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r000_n01.png"));

    /* green psycho fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r045_n01.png"));

    /* green psycho fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r090_n01.png"));

    /* green psycho fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r135_n01.png"));

    /* green psycho fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r180_n01.png"));

    /* green psycho fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r225_n01.png"));

    /* green psycho fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r270_n01.png"));

    /* green psycho fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_green_r315_n01.png"));
}

void GameArea::loadRedPsychoFireAnimations() {
    /* red psycho fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r000_n01.png"));

    /* red psycho fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r045_n01.png"));

    /* red psycho fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r090_n01.png"));

    /* red psycho fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r135_n01.png"));

    /* red psycho fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r180_n01.png"));

    /* red psycho fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r225_n01.png"));

    /* red psycho fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r270_n01.png"));

    /* red psycho fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_red_r315_n01.png"));
}

void GameArea::loadYellowPsychoFireAnimations() {
    /* yellow psycho fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r000_n01.png"));

    /* yellow psycho fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r045_n01.png"));

    /* yellow psycho fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r090_n01.png"));

    /* yellow psycho fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r135_n01.png"));

    /* yellow psycho fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r180_n01.png"));

    /* yellow psycho fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r225_n01.png"));

    /* yellow psycho fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r270_n01.png"));

    /* yellow psycho fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/psycho/fire_yellow_r315_n01.png"));
}

void GameArea::loadPyroFireAnimations() {
    /* blue pyro fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r000_n02.png"));

    /* blue pyro fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r045_n02.png"));

    /* blue pyro fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r090_n02.png"));

    /* blue pyro fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r135_n02.png"));

    /* blue pyro fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r180_n02.png"));

    /* blue pyro fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r225_n02.png"));

    /* blue pyro fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r270_n02.png"));

    /* blue pyro fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/pyro/fire_blue_r315_n02.png"));
}

void GameArea::loadSniperFireAnimations() {
    loadBlueSniperFireAnimations();
    loadGreenSniperFireAnimations();
    loadRedSniperFireAnimations();
    loadYellowSniperFireAnimations();
}

void GameArea::loadBlueSniperFireAnimations() {
    /* blue sniper fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r000_n04.png"));

    /* blue sniper fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r045_n04.png"));

    /* blue sniper fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r090_n04.png"));

    /* blue sniper fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r135_n04.png"));

    /* blue sniper fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r180_n04.png"));

    /* blue sniper fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r225_n04.png"));

    /* blue sniper fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r270_n04.png"));

    /* blue sniper fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenSniperFireAnimations() {
    /* green sniper fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r000_n04.png"));

    /* green sniper fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r045_n04.png"));

    /* green sniper fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r090_n04.png"));

    /* green sniper fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r135_n04.png"));

    /* green sniper fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r180_n04.png"));

    /* green sniper fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r225_n04.png"));

    /* green sniper fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r270_n04.png"));

    /* green sniper fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_green_r315_n04.png"));
}

void GameArea::loadRedSniperFireAnimations() {
    /* red sniper fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r000_n04.png"));

    /* red sniper fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r045_n04.png"));

    /* red sniper fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r090_n04.png"));

    /* red sniper fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r135_n04.png"));

    /* red sniper fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r180_n04.png"));

    /* red sniper fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r225_n04.png"));

    /* red sniper fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r270_n04.png"));

    /* red sniper fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_red_r315_n04.png"));
}

void GameArea::loadYellowSniperFireAnimations() {
    /* yellow sniper fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r000_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r000_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r000_n04.png"));

    /* yellow sniper fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r045_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r045_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r045_n04.png"));

    /* yellow sniper fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r090_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r090_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r090_n04.png"));

    /* yellow sniper fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r135_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r135_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r135_n04.png"));

    /* yellow sniper fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r180_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r180_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r180_n04.png"));

    /* yellow sniper fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r225_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r225_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r225_n04.png"));

    /* yellow sniper fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r270_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r270_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r270_n04.png"));

    /* yellow sniper fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r315_n02.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r315_n03.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/sniper/fire_yellow_r315_n04.png"));
}

void GameArea::loadToughFireAnimations() {
    loadBlueToughFireAnimations();
    loadGreenToughFireAnimations();
    loadRedToughFireAnimations();
    loadYellowToughFireAnimations();
}

void GameArea::loadBlueToughFireAnimations() {
    /* blue tough fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r000_n02.png"));

    /* blue tough fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r045_n02.png"));

    /* blue tough fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r090_n02.png"));

    /* blue tough fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r135_n02.png"));

    /* blue tough fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r180_n02.png"));

    /* blue tough fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r225_n02.png"));

    /* blue tough fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r270_n02.png"));

    /* blue tough fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenToughFireAnimations() {
    /* green tough fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r000_n02.png"));

    /* green tough fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r045_n02.png"));

    /* green tough fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r090_n02.png"));

    /* green tough fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r135_n02.png"));

    /* green tough fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r180_n02.png"));

    /* green tough fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r225_n02.png"));

    /* green tough fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r270_n02.png"));

    /* green tough fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_green_r315_n02.png"));
}

void GameArea::loadRedToughFireAnimations() {
    /* red tough fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r000_n02.png"));

    /* red tough fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r045_n02.png"));

    /* red tough fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r090_n02.png"));

    /* red tough fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r135_n02.png"));

    /* red tough fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r180_n02.png"));

    /* red tough fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r225_n02.png"));

    /* red tough fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r270_n02.png"));

    /* red tough fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_red_r315_n02.png"));
}

void GameArea::loadYellowToughFireAnimations() {
    /* yellow tough fire at 0 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r000_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r000_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_0].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r000_n02.png"));

    /* yellow tough fire at 45 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r045_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r045_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_45].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r045_n02.png"));

    /* yellow tough fire at 90 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r090_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r090_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_90].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r090_n02.png"));

    /* yellow tough fire at 135 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r135_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r135_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r135_n02.png"));

    /* yellow tough fire at 180 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r180_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r180_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r180_n02.png"));

    /* yellow tough fire at 225 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r225_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r225_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r225_n02.png"));

    /* yellow tough fire at 270 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r270_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r270_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r270_n02.png"));

    /* yellow tough fire at 315 degrees */
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r315_n00.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r315_n01.png"));
    unitsFireAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE_315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/tough/fire_yellow_r315_n02.png"));
}

/* WALK animations loading */
void GameArea::loadBlueWalkingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n03.png"));
}

void GameArea::loadGreenWalkingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n03.png"));
}

void GameArea::loadRedWalkingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n03.png"));
}

void GameArea::loadYellowWalkingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n03.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n00.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n01.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n02.png"));
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::WALK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n03.png"));
}

/* STAND animations loading */
void GameArea::loadBlueStandingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r000.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r045.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r090.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r135.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r180.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r225.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r270.png"));

    unitsGeneralAnimations.operator[](TeamEnum::BLUE)[ActionsEnum::STAND_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r315.png"));
}

void GameArea::loadGreenStandingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r000.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r045.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r090.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r135.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r180.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r225.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r270.png"));

    unitsGeneralAnimations.operator[](TeamEnum::GREEN)[ActionsEnum::STAND_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r315.png"));
}

void GameArea::loadRedStandingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r000.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r045.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r090.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r135.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r180.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r225.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r270.png"));

    unitsGeneralAnimations.operator[](TeamEnum::RED)[ActionsEnum::STAND_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r315.png"));
}

void GameArea::loadYellowStandingAnimations() {
    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_0].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r000.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_45].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r045.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_90].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r090.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r135.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r180.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r225.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r270.png"));

    unitsGeneralAnimations.operator[](TeamEnum::YELLOW)[ActionsEnum::STAND_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r315.png"));
}

/* BUILDINGS' animations loading */
void GameArea::loadBuildingsResources() {
    buildings[BuildingsEnum::FORT].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/fort_jungle_front.png"));

    buildings[BuildingsEnum::FORT_DESTROYED].emplace_back(
            Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/fort_jungle_front_destroyed.png"));

    buildings[BuildingsEnum::VEHICLE_FABRIC].emplace_back(
            Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/vehicle/base_jungle.png"));

    buildings[BuildingsEnum::VEHICLE_FABRIC_DESTROYED].emplace_back(
            Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/vehicle/base_destroyed_jungle.png"));

    buildings[BuildingsEnum::ROBOT_FABRIC].emplace_back(
            Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/robot/base_jungle.png"));

    buildings[BuildingsEnum::ROBOT_FABRI_DESTROYEDC].emplace_back(
            Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/robot/base_destroyed_jungle.png"));
}

/* VEHICLES' animations loading */
void GameArea::loadTiresAnimations() {
    loadJeepTires();
}

void GameArea::loadJeepTires() {
    tires[UnitsEnum::JEEP_000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n00.png"));

    tires[UnitsEnum::JEEP_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n01.png"));

    tires[UnitsEnum::JEEP_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n02.png"));

    tires[UnitsEnum::JEEP_180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n03.png"));
}

void GameArea::loadNeuterVehiclesAnimations() {
    loadNeuterJeepAnimations();
}

void GameArea::loadNeuterJeepAnimations() {
    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r000.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r045.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r090.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r135.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r180.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r225.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r270.png"));

    vehicleBases.operator[](TeamEnum::NEUTER)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r315.png"));
}

void GameArea::loadBlueVehiclesAnimations() {
    loadBlueJeepAnimations();
    loadBlueLightTankAnimations();
    loadBlueMediumTankAnimations();
    loadBlueMMLAnimations();
    loadBlueHeavyTankAnimations();
}

void GameArea::loadBlueJeepAnimations() {
    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n01.png"));
}

void GameArea::loadBlueLightTankAnimations() {
    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMediumTankAnimations() {
    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMMLAnimations() {
    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n02.png"));
}

void GameArea::loadBlueHeavyTankAnimations() {
    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n02.png"));
}

void GameArea::loadGreenVehiclesAnimations() {
    loadGreenJeepAnimations();
    loadGreenLightTankAnimations();
    loadGreenMediumTankAnimations();
    loadGreenMMLAnimations();
    loadGreenHeavyTankAnimations();
}

void GameArea::loadGreenJeepAnimations() {
    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n01.png"));
}

void GameArea::loadGreenLightTankAnimations() {
    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMediumTankAnimations() {
    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMMLAnimations() {
    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n02.png"));
}

void GameArea::loadGreenHeavyTankAnimations() {
    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n02.png"));
}

void GameArea::loadRedVehiclesAnimations() {
    loadRedJeepAnimations();
    loadRedLightTankAnimations();
    loadRedMediumTankAnimations();
    loadRedMMLAnimations();
    loadRedHeavyTankAnimations();
}

void GameArea::loadRedJeepAnimations() {
    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n01.png"));
}

void GameArea::loadRedLightTankAnimations() {
    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n02.png"));


    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMediumTankAnimations() {
    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMMLAnimations() {
    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n02.png"));
}

void GameArea::loadRedHeavyTankAnimations() {
    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::RED)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n02.png"));
}

void GameArea::loadYellowVehiclesAnimations() {
    loadYellowJeepAnimations();
    loadYellowLightTankAnimations();
    loadYellowMediumTankAnimations();
    loadYellowMMLAnimations();
    loadYellowHeavyTankAnimations();
}

void GameArea::loadYellowJeepAnimations() {
    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::JEEP_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n01.png"));
}

void GameArea::loadYellowLightTankAnimations() {
    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMediumTankAnimations() {
    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMMLAnimations() {
    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::MML_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowHeavyTankAnimations() {
    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n02.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n00.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n01.png"));

    vehicleBases.operator[](TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK_315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n02.png"));
}

/* BLUE JEEP */
void GameArea::drawBlueJeep000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_000).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_000).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_000).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_000).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_045).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_045).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_045).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_045).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_090).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_090).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_090).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_090).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_135).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_135).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_135).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_135).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_180).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_180).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_180).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_180).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_225).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_225).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_225).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_225).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_270).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_270).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_270).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_270).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueJeep315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_315).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::JEEP_315).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_315).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::JEEP_315).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

/* GREEN JEEP */
void GameArea::drawGreenJeep000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_000).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_000).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_000).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_000).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_045).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_045).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_045).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_045).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_090).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_090).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_090).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_090).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_135).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_135).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_135).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_135).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_180).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_180).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_180).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_180).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_225).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_225).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_225).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_225).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_270).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_270).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_270).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_270).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenJeep315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_315).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::JEEP_315).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_315).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::JEEP_315).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

/* RED JEEP*/
void GameArea::drawRedJeep000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_000).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_000).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_000).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_000).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_045).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_045).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_045).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_045).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_090).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_090).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_090).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_090).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_135).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_135).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_135).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_135).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_180).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_180).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_180).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_180).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_225).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_225).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_225).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_225).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_270).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_270).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_270).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_270).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedJeep315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_315).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::JEEP_315).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_315).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::RED).at(UnitsEnum::JEEP_315).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

/* YELLOW JEEP*/
void GameArea::drawYellowJeep000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_000).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_000).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_000).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_000).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_000).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_000).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_045).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_045).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_045).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_045).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_045).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_045).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_090).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_090).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_090).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_090).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_090).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_090).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_135).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_135).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_135).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_135).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_135).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_135).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_180).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_180).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_180).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_180).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_180).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_180).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_225).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_225).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_225).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_225).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_225).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_225).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_270).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_270).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_270).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_270).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_270).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_270).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowJeep315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    /* first draw tires */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  tires.at(UnitsEnum::JEEP_315).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_width(),
                  tires.at(UnitsEnum::JEEP_315).at(jeepCounter)->get_height());
    cr->fill();

    /* then draw vehicle as a whole */
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::JEEP_315).
                                          at(jeepCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_315).
                          at(jeepCounter)->get_height());
    cr->fill();

    /* update counters */
    tireCounter == tires.at(UnitsEnum::JEEP_315).size()-1 ? (tireCounter = 0)
                                                          : (tireCounter++);

    jeepCounter ==
        vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::JEEP_315).size()-1
    ? (jeepCounter = 0) : (jeepCounter++);
    /* end update counter section */

    cr->restore();
}

/* TANKS DRAWINGS */
/* LIGHT TANK */
/* BLUE LIGHT TANK DRAWING */
void GameArea::drawBlueLightTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueLightTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* GREEN LIGHT TANK DRAWING */
void GameArea::drawGreenLightTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenLightTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* RED LIGHT TANK DRAWING */
void GameArea::drawRedLightTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedLightTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* YELLOW LIGHT TANK DRAWING */
void GameArea::drawYellowLightTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_000).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_000).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_000).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_045).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_045).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_045).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_090).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_090).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_090).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowLightTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::LIGHT_TANK_315).
                                          at(lightTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::LIGHT_TANK_315).
                          at(lightTankCounter)->get_height());
    cr->fill();

    /* update counters */
    lightTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::LIGHT_TANK_315).size()-1
    ? (lightTankCounter = 0) : (lightTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* MEDIUM TANK */
/* BLUE MEDIUM TANK DRAWING */
void GameArea::drawBlueMediumTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMediumTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* GREEN MEDIUM TANK DRAWING */
void GameArea::drawGreenMediumTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMediumTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* RED MEDIUM TANK DRAWING */
void GameArea::drawRedMediumTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMediumTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* YELLOW MEDIUM TANK DRAWING */
void GameArea::drawYellowMediumTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_000).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_000).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_000).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_045).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_045).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_045).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_090).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_090).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_090).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMediumTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MEDIUM_TANK_315).
                                          at(mediumTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::MEDIUM_TANK_315).
                          at(mediumTankCounter)->get_height());
    cr->fill();

    /* update counters */
    mediumTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MEDIUM_TANK_315).size()-1
    ? (mediumTankCounter = 0) : (mediumTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* HEAVY TANK */
/* BLUE HEAVY TANK DRAWING */
void GameArea::drawBlueHeavyTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueHeavyTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* GREEN HEAVY TANK DRAWING */
void GameArea::drawGreenHeavyTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenHeavyTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* RED HEAVY TANK DRAWING */
void GameArea::drawRedHeavyTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedHeavyTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* YELLOW HEAVY TANK DRAWING */
void GameArea::drawYellowHeavyTank000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_000).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_000).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_000).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_045).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_045).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_045).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_090).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_090).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_090).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowHeavyTank315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::HEAVY_TANK_315).
                                          at(heavyTankCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).
                          at(UnitsEnum::HEAVY_TANK_315).
                          at(heavyTankCounter)->get_height());
    cr->fill();

    /* update counters */
    heavyTankCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::HEAVY_TANK_315).size()-1
    ? (heavyTankCounter = 0) : (heavyTankCounter++);
    /* end update counter section */

    cr->restore();
}

/* BLUE MML */
void GameArea::drawBlueMml000(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_000).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_000).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml045(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_045).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_045).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml090(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_090).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_090).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml135(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_135).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_135).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml180(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_180).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_180).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml225(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_225).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_225).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml270(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_270).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_270).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawBlueMml315(const Cairo::RefPtr<Cairo::Context> &cr,
                               unsigned int xCoordinate,
                               unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::BLUE).
                                          at(UnitsEnum::MML_315).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::BLUE).at(UnitsEnum::MML_315).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

/* GREEN MML */
void GameArea::drawGreenMml000(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_000).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_000).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml045(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_045).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_045).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml090(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_090).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_090).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml135(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_135).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_135).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml180(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_180).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_180).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml225(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_225).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_225).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml270(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_270).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_270).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawGreenMml315(const Cairo::RefPtr<Cairo::Context> &cr,
                                unsigned int xCoordinate,
                                unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::GREEN).
                                          at(UnitsEnum::MML_315).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::GREEN).at(UnitsEnum::MML_315).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

/* RED MML*/
void GameArea::drawRedMml000(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_000).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_000).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml045(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_045).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_045).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml090(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_090).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_090).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml135(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_135).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_135).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml180(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_180).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_180).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml225(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_225).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_225).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml270(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_270).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_270).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawRedMml315(const Cairo::RefPtr<Cairo::Context> &cr,
                              unsigned int xCoordinate,
                              unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::RED).
                                          at(UnitsEnum::MML_315).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::RED).at(UnitsEnum::MML_315).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

/* YELLOW MML*/
void GameArea::drawYellowMml000(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_000).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_000).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_000).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml045(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_045).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_045).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_045).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml090(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_090).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_090).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_090).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml135(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_135).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_135).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_135).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml180(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_180).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_180).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_180).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml225(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_225).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_225).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_225).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml270(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_270).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_270).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */
    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_270).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawYellowMml315(const Cairo::RefPtr<Cairo::Context> &cr,
                                 unsigned int xCoordinate,
                                 unsigned int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  vehicleBases.
                                          at(TeamEnum::YELLOW).
                                          at(UnitsEnum::MML_315).
                                          at(mmlCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_width(),
                  vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_315).
                          at(mmlCounter)->get_height());
    cr->fill();

    /* update counters */

    mmlCounter ==
    vehicleBases.at(TeamEnum::YELLOW).at(UnitsEnum::MML_315).size()-1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */

    cr->restore();
}
