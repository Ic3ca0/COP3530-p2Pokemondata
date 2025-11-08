#pragma once

#include "pokemon.h"
#include "../../hashmap/include/pokemon.h"
#include "build.h"

void printPokemon(const bPokemon& p);
void printPokemon(const hPokemon& p);
void printBuild(const PokemonBuild& b);
void maybeShowBuildsForPokemon(const bPokemon& p, const BuildIndex& buildIndex);
