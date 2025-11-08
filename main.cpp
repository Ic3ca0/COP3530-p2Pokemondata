#include <iostream>
#include <limits>
#include <string>
#include <exception>
#include <chrono>

#include "pokemon_index.h"
#include "build_index.h"
#include "pokedex.h"
#include "printing.h"
#include "timing.h"

// template<typename Func>
// auto timedCall(const std::string& label, Func&& f) -> decltype(f()) {
//     auto start = std::chrono::high_resolution_clock::now();
//     auto result = f();
//     auto end = std::chrono::high_resolution_clock::now();

//     auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//     double ms = us / 1000.0;

//     std::cout << "[TIMER] " << label << " took " << ms << " ms\n";

//     return result;
// }

// int main() {
//     try {
//         const std::string POKEMON_CSV = "Pokemon1025.csv";
//         const std::string BUILDS_CSV  = "Pokemon_100k_builds_competitive_moves.csv";
//
//         // --- B+ Tree indexes ---
//         PokemonIndex pokemonIndex;
//         pokemonIndex.loadFromCSV(POKEMON_CSV);
//
//         BuildIndex buildIndex;
//         buildIndex.loadFromCSV(BUILDS_CSV);
//
//         // --- HashMap indexes ---
//         Pokedex pokedex;
//         if (!pokedex.loadFromCSV(POKEMON_CSV)) {
//             std::cerr << "Failed to load Pokémon CSV into Pokedex.\n";
//         }
//
//         BuildDex buildDex;
//         if (!buildDex.loadFromCSV(BUILDS_CSV)) {
//             std::cerr << "Failed to load builds CSV into BuildDex.\n";
//         }
//
//         // --- Choose which data structure ---
//         std::cout << "Use which index implementation?\n";
//         std::cout << "1) B+ Tree\n";
//         std::cout << "2) HashMap\n";
//         std::cout << "Choice: ";
//         int dsChoice;
//         if (!(std::cin >> dsChoice)) return 0;
//         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//
//         bool useBPlus = (dsChoice != 2); // default to B+ unless user picks 2
//
//         std::cout << "Loaded Pokémon and competitive builds using "
//                   << (useBPlus ? "B+ Tree" : "HashMap")
//                   << " indices.\n\n";
//
//         // --- Main CLI loop ---
//         for (;;) {
//             std::cout << "===== Pokemon Search CLI ("
//                       << (useBPlus ? "B+ Tree" : "HashMap") << ") =====\n";
//             std::cout << "1) Look up by ID\n";
//             std::cout << "2) Look up by Name\n";
//             std::cout << "3) List all by Type\n";
//             std::cout << "0) Quit\n";
//             std::cout << "Choice: ";
//
//             int choice;
//             if (!(std::cin >> choice)) break;
//             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//
//             if (choice == 0) {
//                 std::cout << "Goodbye!\n";
//                 break;
//             }
//
//             // ---- LOOKUP BY ID ----
//             if (choice == 1) {
//                 std::cout << "Enter ID: ";
//                 int id;
//                 std::cin >> id;
//                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//
//                 if (useBPlus) {
//                     const Pokemon* p = pokemonIndex.findById(id);
//                     if (!p) {
//                         std::cout << "No Pokémon with ID " << id << " found.\n";
//                         if (pokemonIndex.hasRawId(id)) {
//                             std::cout << "(Debug: CSV DOES contain this ID, "
//                                          "so please double-check the tree and data.)\n";
//                         } else {
//                             std::cout << "(Debug: This ID does NOT appear in the CSV.)\n";
//                         }
//                         std::cout << "\n";
//                         continue;
//                     }
//                     printPokemon(*p);
//                     maybeShowBuildsForPokemon(*p, buildIndex);
//                 } else {
//                     const hPokemon* p = pokedex.byId(id);
//                     if (!p) {
//                         std::cout << "No Pokémon with ID " << id << " found.\n\n";
//                         continue;
//                     }
//                     printPokemon(*p);
//                     maybeShowBuildsForPokemon(*p, buildDex);
//                 }
//
//             // ---- LOOKUP BY NAME ----
//             } else if (choice == 2) {
//                 std::cout << "Enter Name: ";
//                 std::string name;
//                 std::getline(std::cin, name);
//
//                 if (useBPlus) {
//                     auto results = pokemonIndex.findByName(name);
//                     if (results.empty()) {
//                         std::cout << "No Pokémon named \"" << name << "\" found.\n\n";
//                         continue;
//                     }
//
//                     if (results.size() == 1) {
//                         printPokemon(*results[0]);
//                         maybeShowBuildsForPokemon(*results[0], buildIndex);
//                     } else {
//                         std::cout << "Found " << results.size()
//                                   << " Pokémon with that name:\n";
//                         for (std::size_t i = 0; i < results.size(); ++i) {
//                             std::cout << "[" << i << "] ";
//                             printPokemon(*results[i]);
//                         }
//                         std::cout << "Select index to view builds (-1 to skip): ";
//                         int idx;
//                         std::cin >> idx;
//                         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//                         if (idx >= 0 &&
//                             static_cast<std::size_t>(idx) < results.size()) {
//                             maybeShowBuildsForPokemon(*results[static_cast<std::size_t>(idx)], buildIndex);
//                         }
//                     }
//                 } else {
//                     auto results = pokedex.byName(name);
//                     if (results.empty()) {
//                         std::cout << "No Pokémon named \"" << name << "\" found.\n\n";
//                         continue;
//                     }
//
//                     if (results.size() == 1) {
//                         printPokemon(*results[0]);
//                         maybeShowBuildsForPokemon(*results[0], buildDex);
//                     } else {
//                         std::cout << "Found " << results.size()
//                                   << " Pokémon with that name:\n";
//                         for (std::size_t i = 0; i < results.size(); ++i) {
//                             std::cout << "[" << i << "] ";
//                             printPokemon(*results[i]);
//                         }
//                         std::cout << "Select index to view builds (-1 to skip): ";
//                         int idx;
//                         std::cin >> idx;
//                         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//                         if (idx >= 0 &&
//                             static_cast<std::size_t>(idx) < results.size()) {
//                             maybeShowBuildsForPokemon(*results[static_cast<std::size_t>(idx)], buildDex);
//                         }
//                     }
//                 }
//
//             // ---- LOOKUP BY TYPE ----
//             } else if (choice == 3) {
//                 std::cout << "Enter Type (e.g., Fire, Water, Flying): ";
//                 std::string type;
//                 std::getline(std::cin, type);
//
//                 if (useBPlus) {
//                     auto results = pokemonIndex.findByType(type);
//                     if (results.empty()) {
//                         std::cout << "No Pokémon of type \"" << type << "\" found.\n\n";
//                     } else {
//                         std::cout << "Found " << results.size()
//                                   << " Pokémon of type " << type << ":\n";
//                         for (const Pokemon* p : results) {
//                             printPokemon(*p);
//                         }
//                         std::cout << "\n";
//                     }
//                 } else {
//                     auto results = pokedex.byType(type);
//                     if (results.empty()) {
//                         std::cout << "No Pokémon of type \"" << type << "\" found.\n\n";
//                     } else {
//                         std::cout << "Found " << results.size()
//                                   << " Pokémon of type " << type << ":\n";
//                         for (const hPokemon* p : results) {
//                             printPokemon(*p);
//                         }
//                         std::cout << "\n";
//                     }
//                 }
//
//             } else {
//                 std::cout << "Invalid choice.\n\n";
//             }
//         }
//     } catch (const std::exception& ex) {
//         std::cerr << "Fatal error: " << ex.what() << "\n";
//         return 1;
//     }
//
//     return 0;
//}

int main() {
    try {
        const std::string POKEMON_CSV = "Pokemon1025.csv";
        const std::string BUILDS_CSV  = "Pokemon_100k_builds_competitive_moves.csv";

        // --- B+ Tree indexes ---
        PokemonIndex pokemonIndex;
        pokemonIndex.loadFromCSV(POKEMON_CSV);

        BuildIndex buildIndex;
        buildIndex.loadFromCSV(BUILDS_CSV);

        // --- HashMap indexes ---
        Pokedex pokedex;
        if (!pokedex.loadFromCSV(POKEMON_CSV)) {
            std::cerr << "Failed to load Pokémon CSV into Pokedex.\n";
        }

        BuildDex buildDex;
        if (!buildDex.loadFromCSV(BUILDS_CSV)) {
            std::cerr << "Failed to load builds CSV into BuildDex.\n";
        }

        // --- Choose which data structure ---
        std::cout << "Use which index implementation?\n";
        std::cout << "1) B+ Tree\n";
        std::cout << "2) HashMap\n";
        std::cout << "Choice: ";
        int dsChoice;
        if (!(std::cin >> dsChoice)) return 0;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool useBPlus = (dsChoice != 2); // default to B+ unless user explicitly picks 2

        std::cout << "Loaded Pokémon and competitive builds using "
                  << (useBPlus ? "B+ Tree" : "HashMap")
                  << " indices.\n\n";

        // --- Main CLI loop ---
        for (;;) {
            std::cout << "===== Pokemon Search CLI ("
                      << (useBPlus ? "B+ Tree" : "HashMap") << ") =====\n";
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

            // ---- LOOKUP BY ID ----
            if (choice == 1) {
                std::cout << "Enter ID: ";
                int id;
                std::cin >> id;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (useBPlus) {
                    const Pokemon* p = timedCall("B+Tree: findById", [&]{
                        return pokemonIndex.findById(id);
                    });

                    if (!p) {
                        std::cout << "No Pokémon with ID " << id << " found.\n";
                        if (pokemonIndex.hasRawId(id)) {
                            std::cout << "(Debug: CSV DOES contain this ID, "
                                         "so please double-check the tree and data.)\n";
                        } else {
                            std::cout << "(Debug: This ID does NOT appear in the CSV.)\n";
                        }
                        std::cout << "\n";
                        continue;
                    }
                    printPokemon(*p);
                    maybeShowBuildsForPokemon(*p, buildIndex);

                } else {
                    const hPokemon* p = timedCall("HashMap: byId", [&]{
                        return pokedex.byId(id);
                    });

                    if (!p) {
                        std::cout << "No Pokémon with ID " << id << " found.\n\n";
                        continue;
                    }
                    printPokemon(*p);
                    maybeShowBuildsForPokemon(*p, buildDex);
                }

            // ---- LOOKUP BY NAME ----
            } else if (choice == 2) {
                std::cout << "Enter Name: ";
                std::string name;
                std::getline(std::cin, name);

                if (useBPlus) {
                    auto results = timedCall("B+Tree: findByName", [&]{
                        return pokemonIndex.findByName(name);
                    });

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
                        if (idx >= 0 &&
                            static_cast<std::size_t>(idx) < results.size()) {
                            maybeShowBuildsForPokemon(*results[static_cast<std::size_t>(idx)], buildIndex);
                        }
                    }

                } else {
                    auto results = timedCall("HashMap: byName", [&]{
                        return pokedex.byName(name);
                    });

                    if (results.empty()) {
                        std::cout << "No Pokémon named \"" << name << "\" found.\n\n";
                        continue;
                    }

                    if (results.size() == 1) {
                        printPokemon(*results[0]);
                        maybeShowBuildsForPokemon(*results[0], buildDex);
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
                        if (idx >= 0 &&
                            static_cast<std::size_t>(idx) < results.size()) {
                            maybeShowBuildsForPokemon(*results[static_cast<std::size_t>(idx)], buildDex);
                        }
                    }
                }

            // ---- LOOKUP BY TYPE ----
            } else if (choice == 3) {
                std::cout << "Enter Type (e.g., Fire, Water, Flying): ";
                std::string type;
                std::getline(std::cin, type);

                if (useBPlus) {
                    auto results = timedCall("B+Tree: findByType", [&]{
                        return pokemonIndex.findByType(type);
                    });

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
                    auto results = timedCall("HashMap: byType", [&]{
                        return pokedex.byType(type);
                    });

                    if (results.empty()) {
                        std::cout << "No Pokémon of type \"" << type << "\" found.\n\n";
                    } else {
                        std::cout << "Found " << results.size()
                                  << " Pokémon of type " << type << ":\n";
                        for (const hPokemon* p : results) {
                            printPokemon(*p);
                        }
                        std::cout << "\n";
                    }
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
