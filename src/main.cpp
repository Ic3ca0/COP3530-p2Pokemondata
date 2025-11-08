#include <iostream>
#include <limits>
#include <string>

#include "../bplustree/include/pokemon.h"
#include "../bplustree/include/build.h"
#include "../bplustree/include/printing.h"

#include "../hashmap/include/pokedex.h"
#include "../hashmap/include/builddex.h"

using std::cin;
using std::cout;
using std::string;


// load all data structures from CSVs
void loadAllData(const string& pokemonCsv,
                 const string& buildsCsv,
                 PokemonIndex& bplusPokedex,
                 BuildIndex& bplusBuildIndex,
                 Pokedex& hashPokedex,
                 BuildDex& hashBuildDex)
{
    // B+ tree indexes
    bplusPokedex.loadFromCSV("../hashmap/data/Pokemon.csv");
    bplusBuildIndex.loadFromCSV("../hashmap/data/Pokemon_100k_builds_competitive_moves.csv");

    // Hash map indexes
    if (!hashPokedex.loadFromCSV("../data/Pokemon.csv")) {
        throw std::runtime_error("Failed to load Pokedex (hash map) from " + pokemonCsv);
    }
    if (!hashBuildDex.loadFromCSV("../data/Pokemon_100k_builds_competitive_moves.csv")) {
        throw std::runtime_error("Failed to load BuildDex (hash map) from " + buildsCsv);
    }
}


// hashmap - printing competitive builds from BuildDex
static void printHashBuild(const Build& b) {
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

static void maybeShowHashmapBuildsForPokemon(const hPokemon& p, BuildDex buildDex) {
    cout << "See competitive builds for " << p.name << "? (y/n): ";
    char ans;
    if (!(cin >> ans)) return;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (ans == 'y' || ans == 'Y') {
        auto builds = buildDex.byId(p.id);
        if (builds.empty()) {
            builds = buildDex.byName(p.name);
        }

        if (builds.empty()) {
            cout << "No builds found for " << p.name << ".\n\n";
        } else {
            cout << "Found " << builds.size()
                 << " builds for " << p.name << ":\n\n";
            for (const Build* b : builds) {
                printHashBuild(*b);
            }
        }
    } else {
        cout << "\n";
    }
}


// B+ tree menu 
void runBPlusMenu(PokemonIndex& pokemonIndex, BuildIndex& buildIndex) {
    for (;;) {
        cout << "\n=== B+ Tree Menu ===\n";
        cout << "1) Look up by ID\n";
        cout << "2) Look up by Name\n";
        cout << "3) List all by Type\n";
        cout << "0) Back to data structure selection\n";
        cout << "Choice: ";

        int choice;
        if (!(cin >> choice)) return;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "Returning to data structure selection...\n";
            break;
        }

        if (choice == 1) {
            cout << "Enter ID: ";
            int id;
            cin >> id;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            const bPokemon* p = pokemonIndex.findById(id);
            if (!p) {
                cout << "No Pokémon with ID " << id << " found.\n";
                if (pokemonIndex.hasRawId(id)) {
                    cout << "(Debug: CSV DOES contain this ID, "
                            "so double-check data/index.)\n";
                } else {
                    cout << "(Debug: This ID does NOT appear in the CSV.)\n";
                }
                cout << "\n";
                continue;
            }

            printPokemon(*p);
            // B+ build index lookup
            maybeShowBuildsForPokemon(*p, buildIndex);

        } else if (choice == 2) {
            cout << "Enter Name: ";
            string name;
            std::getline(cin, name);

            auto results = pokemonIndex.findByName(name);
            if (results.empty()) {
                cout << "No Pokémon named \"" << name << "\" found.\n\n";
                continue;
            }

            if (results.size() == 1) {
                printPokemon(*results[0]);
                maybeShowBuildsForPokemon(*results[0], buildIndex);
            } else {
                cout << "Found " << results.size()
                     << " Pokémon with that name:\n";
                for (std::size_t i = 0; i < results.size(); ++i) {
                    cout << "[" << i << "] ";
                    printPokemon(*results[i]);
                }
                cout << "Select index to view builds (-1 to skip): ";
                int idx;
                cin >> idx;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (idx >= 0 && static_cast<std::size_t>(idx) < results.size()) {
                    maybeShowBuildsForPokemon(*results[idx], buildIndex);
                }
            }

        } else if (choice == 3) {
            cout << "Enter Type (e.g., Fire, Water, Flying): ";
            string type;
            std::getline(cin, type);

            auto results = pokemonIndex.findByType(type);
            if (results.empty()) {
                cout << "No Pokémon of type \"" << type << "\" found.\n\n";
            } else {
                cout << "Found " << results.size()
                     << " Pokémon of type " << type << ":\n";
                for (std::size_t i = 0; i < results.size(); ++i) {
                    cout << "[" << i << "] ";
                    printPokemon(*results[i]);
                }
                cout << "Select index to view builds (-1 to skip): ";
                int idx;
                cin >> idx;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (idx >= 0 && static_cast<std::size_t>(idx) < results.size()) {
                    // Also offer competitive builds for type results
                    maybeShowBuildsForPokemon(*results[idx], buildIndex);
                }
            }

        } else {
            cout << "Invalid choice.\n\n";
        }
    }
}


// Hash map menu (uses Pokedex + BuildDex)
void runHashMenu(Pokedex& pokedex, BuildDex buildDex) {
    for (;;) {
        cout << "\n=== Hash Map Menu ===\n";
        cout << "1) Look up by ID\n";
        cout << "2) Look up by Name\n";
        cout << "3) List all by Type\n";
        cout << "0) Back to data structure selection\n";
        cout << "Choice: ";

        int choice;
        if (!(cin >> choice)) return;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "Returning to data structure selection...\n";
            break;
        }

        if (choice == 1) {
            cout << "Enter ID: ";
            int id;
            cin >> id;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            const hPokemon* p = pokedex.byId(id);
            if (!p) {
                cout << "No Pokémon with ID " << id << " found (hash map).\n\n";
                continue;
            }

            printPokemon(*p);
            maybeShowHashmapBuildsForPokemon(*p, buildDex);

        } else if (choice == 2) {
            cout << "Enter Name: ";
            string name;
            std::getline(cin, name);

            auto results = pokedex.byName(name);
            if (results.empty()) {
                cout << "No Pokémon named \"" << name
                     << "\" found (hash map).\n\n";
                continue;
            }

            if (results.size() == 1) {
                printPokemon(*results[0]);
                maybeShowHashmapBuildsForPokemon(*results[0], buildDex);
            } else {
                cout << "Found " << results.size()
                     << " Pokémon with that name (hash map):\n";
                for (std::size_t i = 0; i < results.size(); ++i) {
                    cout << "[" << i << "] ";
                    printPokemon(*results[i]);
                }
                cout << "Select index to view builds (-1 to skip): ";
                int idx;
                cin >> idx;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (idx >= 0 && static_cast<std::size_t>(idx) < results.size()) {
                    maybeShowHashmapBuildsForPokemon(*results[idx], buildDex);
                }
            }

        } else if (choice == 3) {
            cout << "Enter Type (e.g., Fire, Water, Flying): ";
            string type;
            std::getline(cin, type);

            auto results = pokedex.byType(type);
            if (results.empty()) {
                cout << "No Pokémon of type \"" << type
                     << "\" found (hash map).\n\n";
            } else {
                cout << "Found " << results.size()
                     << " Pokémon of type " << type << " (hash map):\n";
                for (std::size_t i = 0; i < results.size(); ++i) {
                    cout << "[" << i << "] ";
                    printPokemon(*results[i]);
                }
                cout << "Select index to view builds (-1 to skip): ";
                int idx;
                cin >> idx;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (idx >= 0 && static_cast<std::size_t>(idx) < results.size()) {
                    maybeShowHashmapBuildsForPokemon(*results[idx], buildDex);
                }
            }

        } else {
            cout << "Invalid choice.\n\n";
        }
    }
}


// Main: choose data structure, then run its menu
int main() {
    try {
        const string POKEMON_CSV = "Pokemon1025.csv";
        const string BUILDS_CSV  = "Pokemon_100k_builds_competitive_moves.csv";

        // B+ Tree structures
        PokemonIndex pokemonIndex;
        BuildIndex   buildIndex;

        // Hash map structures
        Pokedex  pokedex;
        BuildDex buildDex;

        // Load everything once
        loadAllData(POKEMON_CSV, BUILDS_CSV,
                    pokemonIndex, buildIndex,
                    pokedex, buildDex);

        cout << "Loaded " << pokemonIndex.count()
             << " Pokémon and competitive builds into both B+ trees and hash maps.\n\n";

        // choose data structure
        for (;;) {
            cout << "===== Data Structure Selection =====\n";
            cout << "1) Use B+ tree indexes\n";
            cout << "2) Use hash map indexes\n";
            cout << "0) Quit\n";
            cout << "Choice: ";

            int dsChoice;
            if (!(cin >> dsChoice)) break;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (dsChoice == 0) {
                cout << "Goodbye!\n";
                break;
            } else if (dsChoice == 1) {
                runBPlusMenu(pokemonIndex, buildIndex);
            } else if (dsChoice == 2) {
                runHashMenu(pokedex, buildDex);
            } else {
                cout << "Invalid choice.\n\n";
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
