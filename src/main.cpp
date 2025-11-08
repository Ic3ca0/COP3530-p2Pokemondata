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