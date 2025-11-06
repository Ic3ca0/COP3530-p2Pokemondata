cmake_minimum_required(VERSION 3.24)
project(PokemonHashMap CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pokemon_app
    src/main.cpp
    src/pokemon.cpp
)

if(MSVC)
  target_compile_options(pokemon_app PRIVATE /W4)
else()
  target_compile_options(pokemon_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
