#ifndef PRINTING_H
#define PRINTING_H

#include "pokemon_index.h"
#include "build_index.h"
#include "pokedex.h"
#include "timing.h"

// Printing helpers
void printPokemon(const Pokemon& p);
void printPokemon(const hPokemon& p);

void printBuild(const PokemonBuild& b);
void printBuild(const Build& b);

// Interactive helpers to show builds
void maybeShowBuildsForPokemon(const Pokemon& p, const BuildIndex& buildIndex);
void maybeShowBuildsForPokemon(const hPokemon& p, BuildDex& buildDex);

#endif
