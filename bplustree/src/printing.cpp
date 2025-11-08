#include "../include/printing.h"

#include <iostream>
#include <limits>

void printPokemon(const bPokemon& p) {
    std::cout << "ID " << p.id << " - " << p.name;
    if (!p.form.empty()) std::cout << " [" << p.form << "]";
    std::cout << "  Type: " << p.type1;
    if (!p.type2.empty()) std::cout << "/" << p.type2;
    std::cout << "  (HP " << p.hp
              << ", Atk " << p.attack
              << ", Def " << p.defense
              << ", SpA " << p.spAtk
              << ", SpD " << p.spDef
              << ", Spe " << p.speed
              << ")\n";
}

void printPokemon(const hPokemon& p) {
    std::cout << "ID " << p.id << " - " << p.name;
    if (!p.form.empty()) std::cout << " [" << p.form << "]";
    std::cout << "  Type: " << p.type1;
    if (!p.type2.empty()) std::cout << "/" << p.type2;
    std::cout << "  (HP " << p.hp
              << ", Atk " << p.atk
              << ", Def " << p.def
              << ", SpA " << p.spAtk
              << ", SpD " << p.spDef
              << ", Spe " << p.spd
              << ")\n";
}

void printBuild(const PokemonBuild& b) {
    std::cout << "Build #" << b.buildId << " for " << b.name;
    if (!b.form.empty()) std::cout << " [" << b.form << "]";
    std::cout << "  Nature: " << b.nature
              << ", Item: " << b.item << "\n";

    std::cout << "  Final Stats: HP " << b.finalHP
              << ", Atk " << b.finalAttack
              << ", Def " << b.finalDefense
              << ", SpA " << b.finalSpAtk
              << ", SpD " << b.finalSpDef
              << ", Spe " << b.finalSpeed << "\n";

    std::cout << "  Moves: " << b.move1;
    if (!b.move2.empty()) std::cout << " / " << b.move2;
    if (!b.move3.empty()) std::cout << " / " << b.move3;
    if (!b.move4.empty()) std::cout << " / " << b.move4;
    std::cout << "\n";
}

void maybeShowBuildsForPokemon(const bPokemon& p, const BuildIndex& buildIndex) {
    std::cout << "See competitive builds for " << p.name << "? (y/n): ";
    char ans;
    if (!(std::cin >> ans)) return;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ans == 'y' || ans == 'Y') {
        auto builds = buildIndex.findByPokemonId(p.id);
        if (builds.empty()) {
            std::cout << "No builds found for " << p.name << ".\n\n";
        } else {
            std::cout << "Found " << builds.size()
                      << " builds for " << p.name << ":\n\n";
            for (const PokemonBuild* b : builds) {
                printBuild(*b);
            }
        }
    } else {
        std::cout << "\n";
    }
}
