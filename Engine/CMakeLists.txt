cmake_minimum_required(VERSION 3.7)

project(engine)

find_package(SDL2 REQUIRED)

add_library(
        engine
        src/Video/Program.cpp
        src/Video/Window.cpp
        src/Video/Texture.cpp
        src/Video/Mesh.cpp

        src/Util/ObjLoader.cpp

        src/Input/SDLInput.cpp
)

target_link_libraries(
        engine
        glad
        stb_image
        SDL2::SDL2
)

target_include_directories(
        engine PUBLIC
        include
        SDL2::SDL2
)
