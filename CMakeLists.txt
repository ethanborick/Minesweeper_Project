cmake_minimum_required(VERSION 3.14)
project(Project_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)

add_executable(Project_3 main.cpp
        welcome_screen.h
        game_window.h
        sprites.h
        board_tile_leaderboard.h)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(Project_3 PRIVATE sfml-graphics)
target_compile_features(Project_3 PRIVATE cxx_std_17)

file(COPY "${CMAKE_SOURCE_DIR}/Project 3 - Minesweeper Spring 2024/files"
        DESTINATION "${CMAKE_BINARY_DIR}")