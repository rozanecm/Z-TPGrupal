#include <gtkmm/builder.h>
#include <utility>
#include <gdkmm.h>
#include <iostream>
#include "GameArea.h"
#include <string>
#include <giomm.h>
#include <vector>


#define TILESIZE 16    //tile width in pixels
#define NUMBER_OF_TILES_TO_SHOW 10

GameArea::GameArea(BaseObjectType *cobject,
                   const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::DrawingArea(cobject),
        flagCounter(0),
        jeepCounter(0),
        tireCounter(0),
        standingRobotCounter(0),
        walkingRobotCounter(0),
        shootingRobotCounter(0),
        tankCounter(0),
        mmlCounter(0),
        playersMonitor(nullptr),
        buildingsMonitor(nullptr),
        mapMonitor(nullptr),
        unitsSelected(false),
        /* camera is initialized with size 0,0 because we don't
         * have this data yet */
        camera(TILESIZE, 0, 0, NUMBER_OF_TILES_TO_SHOW) {
    loadResources();

    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::BUTTON_RELEASE_MASK);
    add_events(Gdk::EventMask::KEY_PRESS_MASK);
    set_can_focus(true);
}

void GameArea::loadResources() {
    try {
        /* load flags animations */
        loadFlagAnimations();
        /* load units resources */
        loadUnitsResources();
        /* load buildings resources */
        loadBuildingsResources();

        /* load some img */
        someImg = Gdk::Pixbuf::create_from_file("res/portraits/grunt.png");

        /* Load tiles */
        tiles["Tierra"] = Gdk::Pixbuf::create_from_file(
                "res/assets/tiles/tierra.png");
        tiles["Agua"] = Gdk::Pixbuf::create_from_file
                ("res/assets/tiles/agua.png");
        tiles["Lava"] = Gdk::Pixbuf::create_from_file(
                "res/assets/tiles/lava.png");
    } catch (Glib::FileError e) {
        std::cerr << e.what();
    }
}

GameArea::~GameArea() {}

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    if (selectionMade){
        makeSelection();
        processSelection();
    }
    drawBaseMap(cr, camera.getPosition());
    drawFlagAnimation(cr, 500, 500);
    //todo implement building drawing
//    drawBuildings();
    drawUnitsInMap(cr);
    updateCounters();
    return true;
}

void GameArea::drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr,
                           std::pair<unsigned int, unsigned int>
                           cameraPosition) {
    /* cameraPosition is given in pixels.
     * i,j indicate TILES. */
    if (mapMonitor->getXSize() == 0 and mapMonitor->getYSize() == 0) {
        return;
    }

    for (unsigned int i = 0; i < NUMBER_OF_TILES_TO_SHOW; ++i) {
        for (unsigned int j = 0; j < NUMBER_OF_TILES_TO_SHOW; ++j) {
            drawTileAt(cr, i, j, mapMonitor->getTerrainTypeAt(
                    cameraPosition.first / TILESIZE -
                    NUMBER_OF_TILES_TO_SHOW / 2 + i,
                    cameraPosition.second / TILESIZE -
                    NUMBER_OF_TILES_TO_SHOW / 2 +
                    j));
        }
    }
}

void GameArea::drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                          unsigned int xCoordinate, unsigned int yCoordinate,
                          std::string terrainType) {
    cr->save();
    auto pixbuf = tiles.find(terrainType);
    if (pixbuf == tiles.end()) {
        return;
    }
    Gdk::Cairo::set_source_pixbuf(cr, pixbuf->second,
                                  xCoordinate * get_width() /
                                  NUMBER_OF_TILES_TO_SHOW,
                                  yCoordinate * get_height() /
                                  NUMBER_OF_TILES_TO_SHOW);

    cr->rectangle(xCoordinate * get_width() / NUMBER_OF_TILES_TO_SHOW,
                  yCoordinate * get_height() / NUMBER_OF_TILES_TO_SHOW,
                  get_width() / NUMBER_OF_TILES_TO_SHOW,
                  get_height() / NUMBER_OF_TILES_TO_SHOW);
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
    } else if (event->keyval == GDK_KEY_Down) {
        camera.moveDown();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Left) {
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Right) {
        camera.moveRight();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Up) {
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

void GameArea::makeSelection() {
    /* tell each of the structures storing objects in the map to mark as
     * selected the items which are within the mouse selection */
    //todo filter out other players' units.
    playersMonitor->markAsSelectedInRange(
            unitsSelected,
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

void GameArea::processSelection() {
    //todo complete this method. It should
    if (xStartCoordinate == xFinishCoordinate and yStartCoordinate ==
                                                          yFinishCoordinate) {
        /* case if user clicked */
        if (unitsSelected){
            processClickWithUnitsSelected();
        } else {
            processClick();
        }
    }
    /* at the end of the selection processing, reset unitsSelection bool */
    unitsSelected = false;
}

void GameArea::processClickWithUnitsSelected() {
//    pseudo code:
//      get clicked unit/building. if clicked building is of own team, select
// it, otherwise atack. If nothing was selected, move selected units to
// clicked position.
}

void GameArea::processClick() {
    if (unitsSelected){

    }
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
void GameArea::loadGruntFireAnimations() {
    loadBlueGruntFireAnimations();
    loadGreenGruntFireAnimations();
    loadRedGruntFireAnimations();
    loadYellowGruntFireAnimations();
}

void GameArea::loadBlueGruntFireAnimations() {
    /* blue grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n04.png"));

    /* blue grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n04.png"));

    /* blue grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n04.png"));

    /* blue grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n04.png"));

    /* blue grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n04.png"));

    /* blue grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n04.png"));

    /* blue grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n04.png"));

    /* blue grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenGruntFireAnimations() {
    /* green grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n04.png"));

    /* green grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n04.png"));

    /* green grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n04.png"));

    /* green grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n04.png"));

    /* green grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n04.png"));

    /* green grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n04.png"));

    /* green grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n04.png"));

    /* green grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n04.png"));
}

void GameArea::loadRedGruntFireAnimations() {
    /* red grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n04.png"));

    /* red grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n04.png"));

    /* red grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n04.png"));

    /* red grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n04.png"));

    /* red grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n04.png"));

    /* red grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n04.png"));

    /* red grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n04.png"));

    /* red grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n04.png"));
}

void GameArea::loadYellowGruntFireAnimations() {
    /* yellow grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n04.png"));

    /* yellow grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n04.png"));

    /* yellow grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n04.png"));

    /* yellow grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n04.png"));

    /* yellow grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n04.png"));

    /* yellow grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n04.png"));

    /* yellow grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n04.png"));

    /* yellow grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
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
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n02.png"));

    /* blue laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n02.png"));

    /* blue laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n02.png"));

    /* blue laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n02.png"));

    /* blue laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n02.png"));

    /* blue laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n02.png"));

    /* blue laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n02.png"));

    /* blue laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenLaserFireAnimations() {
    /* green laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n02.png"));

    /* green laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n02.png"));

    /* green laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n02.png"));

    /* green laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n02.png"));

    /* green laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n02.png"));

    /* green laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n02.png"));

    /* green laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n02.png"));

    /* green laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n02.png"));
}

void GameArea::loadRedLaserFireAnimations() {
    /* red laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n02.png"));

    /* red laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n02.png"));

    /* red laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n02.png"));

    /* red laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n02.png"));

    /* red laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n02.png"));

    /* red laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n02.png"));

    /* red laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n02.png"));

    /* red laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n02.png"));
}

void GameArea::loadYellowLaserFireAnimations() {
    /* yellow laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n02.png"));

    /* yellow laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n02.png"));

    /* yellow laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n02.png"));

    /* yellow laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n02.png"));

    /* yellow laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n02.png"));

    /* yellow laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n02.png"));

    /* yellow laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n02.png"));

    /* yellow laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
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
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r000_n01.png"));

    /* blue psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r045_n01.png"));

    /* blue psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r090_n01.png"));

    /* blue psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r135_n01.png"));

    /* blue psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r180_n01.png"));

    /* blue psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r225_n01.png"));

    /* blue psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r270_n01.png"));

    /* blue psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r315_n01.png"));
}

void GameArea::loadGreenPsychoFireAnimations() {
    /* green psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r000_n01.png"));

    /* green psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r045_n01.png"));

    /* green psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r090_n01.png"));

    /* green psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r135_n01.png"));

    /* green psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r180_n01.png"));

    /* green psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r225_n01.png"));

    /* green psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r270_n01.png"));

    /* green psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r315_n01.png"));
}

void GameArea::loadRedPsychoFireAnimations() {
    /* red psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r000_n01.png"));

    /* red psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r045_n01.png"));

    /* red psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r090_n01.png"));

    /* red psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r135_n01.png"));

    /* red psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r180_n01.png"));

    /* red psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r225_n01.png"));

    /* red psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r270_n01.png"));

    /* red psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r315_n01.png"));
}

void GameArea::loadYellowPsychoFireAnimations() {
    /* yellow psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r000_n01.png"));

    /* yellow psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r045_n01.png"));

    /* yellow psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r090_n01.png"));

    /* yellow psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r135_n01.png"));

    /* yellow psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r180_n01.png"));

    /* yellow psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r225_n01.png"));

    /* yellow psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r270_n01.png"));

    /* yellow psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r315_n01.png"));
}

void GameArea::loadPyroFireAnimations() {
    /* blue pyro fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n02.png"));

    /* blue pyro fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n02.png"));

    /* blue pyro fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n02.png"));

    /* blue pyro fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n02.png"));

    /* blue pyro fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n02.png"));

    /* blue pyro fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n02.png"));

    /* blue pyro fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n02.png"));

    /* blue pyro fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
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
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n04.png"));

    /* blue sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n04.png"));

    /* blue sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n04.png"));

    /* blue sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n04.png"));

    /* blue sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n04.png"));

    /* blue sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n04.png"));

    /* blue sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n04.png"));

    /* blue sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenSniperFireAnimations() {
    /* green sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n04.png"));

    /* green sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n04.png"));

    /* green sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n04.png"));

    /* green sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n04.png"));

    /* green sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n04.png"));

    /* green sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n04.png"));

    /* green sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n04.png"));

    /* green sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n04.png"));
}

void GameArea::loadRedSniperFireAnimations() {
    /* red sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n04.png"));

    /* red sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n04.png"));

    /* red sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n04.png"));

    /* red sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n04.png"));

    /* red sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n04.png"));

    /* red sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n04.png"));

    /* red sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n04.png"));

    /* red sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n04.png"));
}

void GameArea::loadYellowSniperFireAnimations() {
    /* yellow sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n04.png"));

    /* yellow sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n04.png"));

    /* yellow sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n04.png"));

    /* yellow sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n04.png"));

    /* yellow sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n04.png"));

    /* yellow sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n04.png"));

    /* yellow sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n04.png"));

    /* yellow sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
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
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n02.png"));

    /* blue tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n02.png"));

    /* blue tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n02.png"));

    /* blue tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n02.png"));

    /* blue tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n02.png"));

    /* blue tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n02.png"));

    /* blue tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n02.png"));

    /* blue tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenToughFireAnimations() {
    /* green tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n02.png"));

    /* green tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n02.png"));

    /* green tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n02.png"));

    /* green tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n02.png"));

    /* green tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n02.png"));

    /* green tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n02.png"));

    /* green tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n02.png"));

    /* green tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n02.png"));
}

void GameArea::loadRedToughFireAnimations() {
    /* red tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n02.png"));

    /* red tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n02.png"));

    /* red tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n02.png"));

    /* red tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n02.png"));

    /* red tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n02.png"));

    /* red tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n02.png"));

    /* red tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n02.png"));

    /* red tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n02.png"));
}

void GameArea::loadYellowToughFireAnimations() {
    /* yellow tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n02.png"));

    /* yellow tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n02.png"));

    /* yellow tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n02.png"));

    /* yellow tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n02.png"));

    /* yellow tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n02.png"));

    /* yellow tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n02.png"));

    /* yellow tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n02.png"));

    /* yellow tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n02.png"));
}

/* MOVE animations loading */
void GameArea::loadBlueWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n03.png"));
}

void GameArea::loadGreenWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n03.png"));
}

void GameArea::loadRedWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n03.png"));
}

void GameArea::loadYellowWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n03.png"));
}

/* STAND animations loading */
void GameArea::loadBlueStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r315.png"));
}

void GameArea::loadGreenStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r315.png"));
}

void GameArea::loadRedStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r315.png"));
}

void GameArea::loadYellowStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r315.png"));
}

/* BUILDINGS' animations loading */
void GameArea::loadBuildingsResources() {
    buildings[BuildingsEnum::FORT].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/fort_jungle_front.png"));

    buildings[BuildingsEnum::FORT_DESTROYED].
            emplace_back(Gdk::Pixbuf::create_from_file(
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
    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n00.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n01.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n02.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n03.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n00.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n01.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n02.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n03.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n00.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n01.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n02.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n03.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n00.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n01.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n02.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n03.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n00.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n01.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n02.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n03.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n00.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n01.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n02.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n03.png"));
}

void GameArea::loadNeuterVehiclesAnimations() {
    loadNeuterJeepAnimations();
}

void GameArea::loadNeuterJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
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
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n01.png"));
}

void GameArea::loadBlueLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n02.png"));
}

void GameArea::loadBlueHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
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
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n01.png"));
}

void GameArea::loadGreenLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n02.png"));
}

void GameArea::loadGreenHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
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
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n01.png"));
}

void GameArea::loadRedLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n02.png"));
}

void GameArea::loadRedHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
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
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n01.png"));
}

void GameArea::loadYellowLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n02.png"));
}

void GameArea::drawJeepTires(const Cairo::RefPtr<Cairo::Context> &cr,
                             unsigned int xCoordinate, unsigned int yCoordinate,
                             RotationsEnum rotation) {
    cr->save();
    /* first draw jeepTires */
    Gdk::Cairo::set_source_pixbuf(cr, jeepTires.at(rotation).at(tireCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  jeepTires.at(rotation).at(tireCounter)->get_width(),
                  jeepTires.at(rotation).at(tireCounter)->get_height());
    cr->fill();

    /* update counter */
    tireCounter == jeepTires.size() - 1 ? (tireCounter = 0) : (tireCounter++);
    /* end update counter section */

    cr->restore();
}

void GameArea::drawUnit(TeamEnum team, UnitsEnum unitType,
                        ActionsEnum actionType,
                        RotationsEnum rotation, unsigned short unitCounter,
                        const Cairo::RefPtr<Cairo::Context> &cr,
                        unsigned int xCoordinate, unsigned int yCoordinate) {
    cr->save();
    /* adapt given data to savd imgs. Applies to vehicles */
    if (unitType == UnitsEnum::JEEP &&
        rotation != RotationsEnum::r090 &&
        rotation != RotationsEnum::r270) {
        /* rotations 090 and 270 dont have tires */
        drawJeepTires(cr, xCoordinate, yCoordinate, rotation);
    }
    if (unitType == UnitsEnum::HEAVY_TANK
        or unitType == UnitsEnum::LIGHT_TANK
        or unitType == UnitsEnum::MEDIUM_TANK
        or unitType == UnitsEnum::MML) {
        /* same assets are used for given rotations;
         * e.g.: 135 and 315 are drawn with same img */
        if (rotation == RotationsEnum::r135) {
            rotation = RotationsEnum::r315;
        } else if (rotation == RotationsEnum::r180) {
            rotation = RotationsEnum::r000;
        } else if (rotation == RotationsEnum::r225) {
            rotation = RotationsEnum::r045;
        } else if (rotation == RotationsEnum::r270) {
            rotation = RotationsEnum::r090;
        }
    } else if ((unitType == UnitsEnum::GRUNT
                or unitType == UnitsEnum::LASER
                or unitType == UnitsEnum::PSYCHO
                or unitType == UnitsEnum::PYRO
                or unitType == UnitsEnum::SNIPER
                or unitType == UnitsEnum::TOUGH)
               and(actionType == ActionsEnum::MOVE
                    or actionType == ActionsEnum::STAND)) {
        /* because same imgs are used to draw all different types of robots
         * when these are moving or standing still, if this is the case, we
         * set the unit type to generic robot */
        unitType = UnitsEnum::GENERIC_ROBOT;
    }

    /* perform actual drawing */
    Gdk::Cairo::set_source_pixbuf(cr, unitsAnimations.at(team).at(unitType).
                                          at(actionType).at(rotation).
                                          at(unitCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate, unitsAnimations.at(team).
                          at(unitType).
                          at(actionType).at(rotation).
                          at(unitCounter)->get_width(),
                  unitsAnimations.at(team).at(unitType).at(actionType).
                          at(rotation).
                          at(unitCounter)->get_height());
    cr->fill();
    cr->restore();
}

void GameArea::drawUnitsInMap(const Cairo::RefPtr<Cairo::Context> &cr) {
    /* pointers (Unit*) are not used here because we are working with a shared
     * resource. This way, we copy the units we want to draw in a protected way,
     * and then we can draw without blocking other code. */
    std::vector<Unit> unitsToDraw = playersMonitor->getUnitsToDraw(
            camera.getPosition().first - NUMBER_OF_TILES_TO_SHOW * TILESIZE,
            camera.getPosition().first + NUMBER_OF_TILES_TO_SHOW * TILESIZE,
            camera.getPosition().second - NUMBER_OF_TILES_TO_SHOW * TILESIZE,
            camera.getPosition().second + NUMBER_OF_TILES_TO_SHOW * TILESIZE);

    for (auto &unit : unitsToDraw) {
        /* check what is being drawn, and choose the counter approprately. */
        unsigned short counter;
        if (unit.getType() == UnitsEnum::JEEP) {
            counter = jeepCounter;
        } else if (unit.getType() == UnitsEnum::LIGHT_TANK or
                   unit.getType() == UnitsEnum::MEDIUM_TANK or
                   unit.getType() == UnitsEnum::HEAVY_TANK) {
            counter = tankCounter;
        } else if (unit.getAction() == ActionsEnum::FIRE) {
            counter = shootingRobotCounter;
        } else if (unit.getAction() == ActionsEnum::MOVE) {
            counter = walkingRobotCounter;
        } else {
            counter = standingRobotCounter;
        }

        /* call actual drawing method */
        drawUnit(unit.getTeam(), unit.getType(), unit.getAction(),
                 unit.getRotation(),
                 counter, cr,
                 camera.mapToCameraYCoordinate(unit.getYCoordinate()),
                 camera.mapToCameraXCoordinate(unit.getXCoordinate()));
    }
    unitsToDraw.clear();
}

void GameArea::updateCounters() {
    /* update units counters */
    flagCounter == flags.at(FlagEnum::BLUE).size() - 1 ? (flagCounter = 0)
                                                       : (flagCounter++);

    shootingRobotCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::PSYCHO).
            at(ActionsEnum::FIRE).size() - 1
    ? (shootingRobotCounter = 0) : (shootingRobotCounter++);

    walkingRobotCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::PSYCHO).
            at(ActionsEnum::MOVE).size() - 1
    ? (walkingRobotCounter = 0) : (walkingRobotCounter++);

    standingRobotCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::PSYCHO).
            at(ActionsEnum::STAND).size() - 1
    ? (standingRobotCounter = 0) : (standingRobotCounter++);

    jeepCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::JEEP).
            at(ActionsEnum::STAND).size() - 1
    ? (jeepCounter = 0) : (jeepCounter++);

    tireCounter ==
    jeepTires.at(RotationsEnum::r000).size() - 1
    ? (tireCounter = 0) : (tireCounter++);

    tankCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::LIGHT_TANK).
            at(ActionsEnum::STAND).size() - 1
    ? (tankCounter = 0) : (tankCounter++);

    mmlCounter ==
    unitsAnimations.at(TeamEnum::BLUE).at(UnitsEnum::HEAVY_TANK).
            at(ActionsEnum::STAND).size() - 1
    ? (mmlCounter = 0) : (mmlCounter++);
    /* end update counter section */
}
