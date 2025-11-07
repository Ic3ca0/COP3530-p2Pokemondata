#pragma once
#include "hashmap.h"
#include "pokemon.h"
#include <algorithm>
#include <fstream>
#include <sstream>

class Pokedex {
    // Main index: ID
    // Secondary indexes: name, type 
    //  - Need collisions for names due to multiple forms
    //      - Ex: Nidoran (M/F), Deoxys (Normal/Attack Form)
    //  - Need collisions for types (obviously)
    HashMap<int, Pokemon> idMap;
    HashMap<string, vector<int>> nameIdx; // Map name to all IDs w/ that name
    HashMap<string, vector<int>> typeIdx; // Map types to all IDs w/ that type

    /* Helpers */
    string lower(string s) const;
    string normalize(string s);
    void removeFromVector(vector<int>& v, int id);
public:
    // Constructor unnecessary b/c HashMap has default one
    /* Core Functions */
    bool upsert(const Pokemon& p);
    bool erase(int id);

    /* Queries */
    const Pokemon* byId(int id);
    vector<const Pokemon*> byName(const string& name);
    vector<const Pokemon*> byType(const string& type);

    /* File Input */
    bool loadFromCSV(const string& path);
};