cmake_minimum_required(VERSION 3.10)

project(lox VERSION 0.0.1 LANGUAGES C)

add_executable(clox src/chunk.c src/memory.c src/debug.c src/value.c src/main.c)
