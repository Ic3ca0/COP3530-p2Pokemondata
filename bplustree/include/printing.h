#pragma once

#include "pokemon.h"
#include "build.h"

void printPokemon(const bPokemon& p);
void printBuild(const PokemonBuild& b);
void maybeShowBuildsForPokemon(const bPokemon& p, const BuildIndex& buildIndex);
