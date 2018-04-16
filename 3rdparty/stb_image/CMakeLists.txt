cmake_minimum_required(VERSION 3.7)

project(stb_image_atom)

add_library(stb_image STATIC src/impl.c)

target_include_directories(stb_image PUBLIC include)