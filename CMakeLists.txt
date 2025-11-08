cmake_minimum_required(VERSION 3.29)
project(P2_DSA_Pokemon_comp)

set(CMAKE_CXX_STANDARD 20)

add_executable(P2_DSA_Pokemon_comp main.cpp
        util.cpp
        util.h
        BPlusTree.h
        HashMap.h
        pokemon_index.h
        pokemon_index.cpp
        build_index.h
        build_index.cpp
        pokedex.h
        pokedex.cpp
        printing.h
        printing.cpp)
