cmake_minimum_required(VERSION 4.0)
project(untitled C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled main.c
        bitops.c
        bitops.h
        game.h
        game.c
        io.h
        io.c
)
