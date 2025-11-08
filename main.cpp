#include <iostream>
#include <limits>

#include "pokemon.h"
#include "build.h"
#include "printing.h"

int main() {
    try {
        const std::string POKEMON_CSV = "Pokemon1025.csv";
        const std::string BUILDS_CSV  = "Pokemon_100k_builds_competitive_moves.csv";

        PokemonIndex pokemonIndex;
        pokemonIndex.loadFromCSV(POKEMON_CSV);

        BuildIndex buildIndex;
        buildIndex.loadFromCSV(BUILDS_CSV);

        std::cout << "Loaded " << pokemonIndex.count()
                  << " Pokémon and competitive builds.\n\n";

        for (;;) {
            std::cout << "===== Pokemon B+ Tree CLI =====\n";
            std::cout << "1) Look up by ID\n";
            std::cout << "2) Look up by Name\n";
            std::cout << "3) List all by Type\n";
            std::cout << "0) Quit\n";
            std::cout << "Choice: ";

            int choice;
            if (!(std::cin >> choice)) break;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice == 0) {
                std::cout << "Goodbye!\n";
                break;
            }

            if (choice == 1) {
                std::cout << "Enter ID: ";
                int id;
                std::cin >> id;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                const Pokemon* p = pokemonIndex.findById(id);
                if (!p) {
                    std::cout << "No Pokémon with ID " << id << " found.\n";
                    if (pokemonIndex.hasRawId(id)) {
                        std::cout << "(Debug: CSV DOES contain this ID, "
                                     "so double-check data/index.)\n";
                    } else {
                        std::cout << "(Debug: This ID does NOT appear in the CSV.)\n";
                    }
                    std::cout << "\n";
                    continue;
                }

                printPokemon(*p);
                maybeShowBuildsForPokemon(*p, buildIndex);
            } else if (choice == 2) {
                std::cout << "Enter Name: ";
                std::string name;
                std::getline(std::cin, name);

                auto results = pokemonIndex.findByName(name);
                if (results.empty()) {
                    std::cout << "No Pokémon named \"" << name << "\" found.\n\n";
                    continue;
                }

                if (results.size() == 1) {
                    printPokemon(*results[0]);
                    maybeShowBuildsForPokemon(*results[0], buildIndex);
                } else {
                    std::cout << "Found " << results.size()
                              << " Pokémon with that name:\n";
                    for (std::size_t i = 0; i < results.size(); ++i) {
                        std::cout << "[" << i << "] ";
                        printPokemon(*results[i]);
                    }
                    std::cout << "Select index to view builds (-1 to skip): ";
                    int idx;
                    std::cin >> idx;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (idx >= 0 && static_cast<std::size_t>(idx) < results.size()) {
                        maybeShowBuildsForPokemon(*results[idx], buildIndex);
                    }
                }
            } else if (choice == 3) {
                std::cout << "Enter Type (e.g., Fire, Water, Flying): ";
                std::string type;
                std::getline(std::cin, type);

                auto results = pokemonIndex.findByType(type);
                if (results.empty()) {
                    std::cout << "No Pokémon of type \"" << type << "\" found.\n\n";
                } else {
                    std::cout << "Found " << results.size()
                              << " Pokémon of type " << type << ":\n";
                    for (const Pokemon* p : results) {
                        printPokemon(*p);
                    }
                    std::cout << "\n";
                }
            } else {
                std::cout << "Invalid choice.\n\n";
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
