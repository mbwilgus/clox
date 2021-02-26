cmake_minimum_required(VERSION 3.10)

project(lox VERSION 0.0.1 LANGUAGES C)

add_executable(clox src/chunk.c src/compiler.c src/debug.c src/memory.c
	src/object.c src/scanner.c src/table.c src/value.c src/vm.c src/main.c)
