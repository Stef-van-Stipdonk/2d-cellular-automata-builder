cmake_minimum_required(VERSION 3.22)
project(Cellular_automata_builder)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(builder "./src/main.c")

target_link_libraries(builder PRIVATE ${SDL2_LIBRARIES})
