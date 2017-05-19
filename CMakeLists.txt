cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)
set(CMAKE_CXX_STANDARD 11)

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
        ${SDL2_mixer_INCLUDE_DIRS})


set(SOURCE_FILES
        src/main.cpp src/GameWindow.cpp src/GameWindow.h
        libs/xml/pugixml.cpp libs/xml/pugiconfig.hpp libs/xml/pugixml.hpp)

add_compile_options(${SDL2_mixer_LDFLAGS} ${SDL2_mixer_CFLAGS})
add_executable(Z_TPGrupal ${SOURCE_FILES})


target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDL2_mixer_LIBRARIES})