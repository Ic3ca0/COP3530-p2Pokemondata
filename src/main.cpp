#include "../include/pokedex.h"
#include <iomanip>
#include <iostream>

/* Prototypes */
void printPokemon(const Pokemon* p);

/* Main */
int main() {
    Pokedex dex;
    string filePath = "../data/Pokemon.csv";

    cout << "Loading Pokémon CSV...\n";
    dex.loadFromCSV(filePath);
    cout << "Pokédex loaded successfully.\n\n";

    // === Test 1: Search by ID ===
    cout << "=== Search by ID ===\n";
    int id = 6; // Charizard, based on your example CSV
    const Pokemon* p = dex.byId(id);
    printPokemon(p);

    // === Test 2: Search by Name ===
    cout << "=== Search by Name ===\n";
    string name = "Bulbasaur";
    vector<const Pokemon*> results = dex.byName(name);
    if (results.empty()) cout << "No Pokémon found with name " << name << ".\n";
    else {
        for (auto* poke : results) printPokemon(poke);
    }

    // === Test 3: Search by Type ===
    cout << "=== Search by Type ===\n";
    string type = "Fire";
    vector<const Pokemon*> fireTypes = dex.byType(type);
    cout << "Pokémon with type " << type << ":\n";
    for (auto* poke : fireTypes)
        cout << " - " << poke->name << " (ID " << poke->id << ")\n";
    cout << endl;

    // === Test 4: Erase a Pokémon ===
    cout << "=== Erase Test ===\n";
    int eraseId = 4; // Charmander
    bool removed = dex.erase(eraseId);
    cout << "Erased ID " << eraseId << "? " << (removed ? "Yes" : "No") << "\n";

    cout << "Try finding Charmander after erase:\n";
    printPokemon(dex.byId(eraseId));
    
    return 0;
}

/* Definitions */
void printPokemon(const Pokemon* p) {
    if (!p) {
        cout << "Pokemon not found.\n";
        return;
    }
    cout << "ID: " << p->id << "\n"
         << "Name: " << p->name << "\n"
         << "Type: " << p->type1;
    if (!p->type2.empty()) cout << "/" << p->type2;
    cout << "\n"
         << "Stats: " << p->total << " total (" 
         << p->hp << " HP, "
         << p->atk << " Atk, "
         << p->def << " Def, "
         << p->spAtk << " SpAtk, "
         << p->spDef << " SpDef, "
         << p->spd << " Spd)\n"
         << "Generation: " << p->gen << "\n\n";
}