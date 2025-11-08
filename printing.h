#pragma once

#include "pokemon.h"
#include "build.h"

void printPokemon(const Pokemon& p);
void printBuild(const PokemonBuild& b);
void maybeShowBuildsForPokemon(const Pokemon& p, const BuildIndex& buildIndex);
