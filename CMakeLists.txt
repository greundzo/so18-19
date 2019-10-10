cmake_minimum_required(VERSION 3.15)
project(so1920)

set(CMAKE_CXX_STANDARD 14)

add_executable(so1920
        src/student.c src/shared.h
        src/gestore.c src/shared.c)
