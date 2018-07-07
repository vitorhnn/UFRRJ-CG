cmake_minimum_required(VERSION 3.7)

project(UFRRJ-CG)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

find_package(ASSIMP REQUIRED)

add_subdirectory(3rdparty)
add_subdirectory(Engine)

add_executable(ufrrj-cg main.cpp)
target_include_directories(ufrrj-cg PRIVATE SDL2::SDL2 ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(ufrrj-cg glad engine SDL2::SDL2 ${ASSIMP_LIBRARIES})
