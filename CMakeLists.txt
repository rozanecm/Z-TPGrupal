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
        src/Thread.cpp src/BuildingPanel.cpp src/BuildingPanel.h)

add_compile_options(${SDL2_mixer_LDFLAGS} ${SDL2_mixer_CFLAGS})
add_executable(Z_TPGrupal ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDL2_mixer_LIBRARIES}
        Threads::Threads)