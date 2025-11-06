#include "../include/pokemon.h"
#include <fstream>
#include <iostream>
#include <sstream>

void loadPokemonCSV(const string& fileName, PokemonHashMap& map);

int main() {
    PokemonHashMap pokedex(256, 0.75f);
    
    loadPokemonCSV("../data/Pokemon.csv", pokedex);

    return 0;
}

void loadPokemonCSV(const string& fileName, PokemonHashMap& map) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << fileName << "\n";
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Pokemon p;
        string cell; // Placeholder for integer values

        // Parse through each column
        getline(ss, cell, ','); p.id = stoi(cell);
        getline(ss, p.name, ',');
        getline(ss, p.form, ',');
        getline(ss, p.type1, ',');
        getline(ss, p.type2, ',');
        getline(ss, cell, ','); p.total = stoi(cell);
        getline(ss, cell, ','); p.hp = stoi(cell);
        getline(ss, cell, ','); p.atk = stoi(cell);
        getline(ss, cell, ','); p.def = stoi(cell);
        getline(ss, cell, ','); p.spAtk = stoi(cell);
        getline(ss, cell, ','); p.spDef = stoi(cell);
        getline(ss, cell, ','); p.spd = stoi(cell);
        getline(ss, cell, ','); p.gen = stoi(cell);

        map.insert(p);
    }
    file.close();
    cout << "Loaded " << map.size() << " Pokemon into the hash map.\n";
}