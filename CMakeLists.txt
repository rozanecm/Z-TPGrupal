set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(LIBS_DIR libs)
set(PUGI ${LIBS_DIR}/xml)

find_package(PkgConfig)

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
        ${SDL2_mixer_INCLUDE_DIRS}
        src/common)

set(SOURCE_FILES
        libs/xml/pugiconfig.hpp
        libs/xml/pugixml.cpp
        libs/xml/pugixml.hpp
        src/client/Armament.cpp
        src/client/Armament.h
        src/client/Building.cpp
        src/client/Building.h
        src/client/BuildingPanel.cpp
        src/client/BuildingPanel.h
        src/client/BuildingsMonitor.cpp
        src/client/BuildingsMonitor.h
        src/client/Cell.cpp
        src/client/Cell.h
        src/client/ClientThread.cpp
        src/client/ClientThread.h
        src/client/GameArea.cpp
        src/client/GameArea.h
        src/client/GameBuilder.cpp
        src/client/GameBuilder.h
        src/client/GameWindow.cpp
        src/client/GameWindow.h
        src/client/GraphicsThread.cpp
        src/client/GraphicsThread.h
        src/client/main.cpp
        src/client/Map.cpp
        src/client/Map.h
        src/client/MapMonitor.cpp
        src/client/MapMonitor.h
        src/client/Player.cpp
        src/client/Player.h
        src/client/PlayersMonitor.cpp
        src/client/PlayersMonitor.h
        src/client/Unit.cpp
        src/client/Unit.h
        src/Lock.cpp
        src/Lock.h
        src/Thread.cpp
        src/Thread.h
        src/client/Camera.cpp
        src/client/Camera.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(src/generator)