#include "printing.h"

#include <iostream>
#include <limits>

using std::cout;
using std::cin;
using std::numeric_limits;
using std::streamsize;

void printPokemon(const Pokemon& p) {
    cout << "ID " << p.id << " - " << p.name;
    if (!p.form.empty()) cout << " [" << p.form << "]";
    cout << "  Type: " << p.type1;
    if (!p.type2.empty()) cout << "/" << p.type2;
    cout << "  (HP " << p.hp
         << ", Atk " << p.attack
         << ", Def " << p.defense
         << ", SpA " << p.spAtk
         << ", SpD " << p.spDef
         << ", Spe " << p.speed
         << ")\n";
}

void printPokemon(const hPokemon& p) {
    cout << "ID " << p.id << " - " << p.name;
    if (!p.form.empty()) cout << " [" << p.form << "]";
    cout << "  Type: " << p.type1;
    if (!p.type2.empty()) cout << "/" << p.type2;
    cout << "  (HP " << p.hp
         << ", Atk " << p.atk
         << ", Def " << p.def
         << ", SpA " << p.spAtk
         << ", SpD " << p.spDef
         << ", Spe " << p.spd
         << ")\n";
}

void printBuild(const PokemonBuild& b) {
    cout << "Build #" << b.buildId << " for " << b.name;
    if (!b.form.empty()) cout << " [" << b.form << "]";
    cout << "  Nature: " << b.nature
         << ", Item: " << b.item << "\n";

    cout << "  Final Stats: HP " << b.finalHP
         << ", Atk " << b.finalAttack
         << ", Def " << b.finalDefense
         << ", SpA " << b.finalSpAtk
         << ", SpD " << b.finalSpDef
         << ", Spe " << b.finalSpeed << "\n";

    cout << "  Moves: " << b.move1;
    if (!b.move2.empty()) cout << " / " << b.move2;
    if (!b.move3.empty()) cout << " / " << b.move3;
    if (!b.move4.empty()) cout << " / " << b.move4;
    cout << "\n";
}

void printBuild(const Build& b) {
    cout << "Build #" << b.buildId << " for " << b.name;
    if (!b.form.empty()) cout << " [" << b.form << "]";
    cout << "  Nature: " << b.nature
         << ", Item: " << b.item << "\n";

    cout << "  Final Stats: HP " << b.f_hp
         << ", Atk " << b.f_atk
         << ", Def " << b.f_def
         << ", SpA " << b.f_spAtk
         << ", SpD " << b.f_spDef
         << ", Spe " << b.f_spd << "\n";

    cout << "  Moves: " << b.move1;
    if (!b.move2.empty()) cout << " / " << b.move2;
    if (!b.move3.empty()) cout << " / " << b.move3;
    if (!b.move4.empty()) cout << " / " << b.move4;
    cout << "\n";
}

void maybeShowBuildsForPokemon(const Pokemon& p, const BuildIndex& buildIndex) {
    std::cout << "See competitive builds for " << p.name << "? (y/n): ";
    char ans;
    if (!(std::cin >> ans)) return;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ans == 'y' || ans == 'Y') {
        auto builds = timedCall("B+Tree: buildsByPokemonId", [&]{
            return buildIndex.findByPokemonId(p.id);
        });

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

void maybeShowBuildsForPokemon(const hPokemon& p, BuildDex& buildDex) {
    std::cout << "See competitive builds for " << p.name << "? (y/n): ";
    char ans;
    if (!(std::cin >> ans)) return;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ans == 'y' || ans == 'Y') {
        auto builds = timedCall("HashMap: buildsByPokemonId", [&]{
            return buildDex.byId(p.id);
        });

        if (builds.empty()) {
            std::cout << "No builds found for " << p.name << ".\n\n";
        } else {
            std::cout << "Found " << builds.size()
                      << " builds for " << p.name << ":\n\n";
            for (const Build* b : builds) {
                printBuild(*b);
            }
        }
    } else {
        std::cout << "\n";
    }
}
