cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)
set(CMAKE_CXX_STANDARD 11)

set(LIBS_DIR libs)
set(PUGI ${LIBS_DIR}/xml)

find_package(PkgConfig)

<<<<<<< HEAD
set(LIBS_DIR src)
set(PUGI ${LIBS_DIR}/xml)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        src/server/main.cpp src/server/cell.cpp
        src/server/cell.h src/server/Terrain.cpp src/server/Terrain.h src/server/Occupant.cpp src/server/Occupant.h
        src/xml/pugixml.cpp src/xml/pugiconfig.hpp src/xml/pugixml.hpp
        src/server/MapLoader.cpp src/server/MapLoader.h src/server/compass.cpp src/server/compass.h
        src/server/lock.cpp src/server/lock.h src/server/lock.cpp src/server/lock.h src/server/node.cpp src/server/node.h
        src/server/size.cpp src/server/size.h src/server/map.cpp src/server/map.h src/server/unit.cpp src/server/unit.h
        src/server/position.cpp src/server/position.h src/server/weapon.cpp src/server/weapon.h
        src/server/unitMold.cpp src/server/unitMold.h src/server/teamable.cpp src/server/teamable.h
        src/server/factory.cpp src/server/factory.h src/server/bullet.cpp src/server/bullet.h
        src/server/thread.cpp src/server/thread.h src/server/socket.cpp src/server/socket.h src/server/socketError.cpp
        src/server/socketError.h src/server/messenger.cpp src/server/messenger.h src/server/controUnit.cpp src/server/controUnit.h)
=======
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(SDL2 sdl2)
pkg_check_modules(SDL2_mixer SDL2_mixer)

link_directories(
        ${SDL2_mixer_LIBRARY_DIRS}
        ${GTKMM_LIBRARY_DIRS})


include_directories(
        ${PUGI}
        ${GTKMM_INCLUDE_DIRS}
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_mixer_INCLUDE_DIRS})


set(SOURCE_FILES
        src/GameBuilder.cpp
        src/GameBuilder.h
        src/main.cpp
        src/GameWindow.cpp
        src/GameWindow.h
        libs/xml/pugixml.cpp
        libs/xml/pugiconfig.hpp
        libs/xml/pugixml.hpp
        src/GameArea.cpp
        src/GameArea.h
        src/GraphicsThread.cpp
        src/GraphicsThread.h
        src/Thread.h
        src/Thread.cpp
        src/Player.cpp
        src/Player.h
        src/Unit.cpp
        src/Unit.h
        src/Armament.cpp
        src/Armament.h
        src/Building.cpp
        src/Building.h
        src/ClientThread.cpp
        src/ClientThread.h
        src/PlayersMonitor.cpp
        src/PlayersMonitor.h
        src/BuildingsMonitor.cpp
        src/BuildingsMonitor.h
        src/Map.cpp
        src/Map.h
        src/Cell.cpp
        src/Cell.h
        src/MapMonitor.cpp
        src/MapMonitor.h
        src/Lock.cpp
        src/Lock.h
        src/Thread.cpp
        src/BuildingPanel.cpp src/BuildingPanel.h)
>>>>>>> 559da496e013878eb8ac6f96fabe233326a33876

add_compile_options(${SDL2_mixer_LDFLAGS} ${SDL2_mixer_CFLAGS})
add_executable(Z_TPGrupal ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


target_link_libraries(Z_TPGrupal
<<<<<<< HEAD
=======
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDL2_mixer_LIBRARIES}
>>>>>>> 559da496e013878eb8ac6f96fabe233326a33876
        Threads::Threads)