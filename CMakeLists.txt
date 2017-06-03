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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(src/generator)