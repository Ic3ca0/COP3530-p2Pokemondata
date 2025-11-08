#include "../include/pokedex.h"

/* Helpers */
//  - Converts to strings to lowercase only
//  - Used in lookups
string Pokedex::lower(string s) const {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = tolower((unsigned char)s[i]);
    return s;
}
//  - Converts to lowercase and trims
//  - Used in insertions
string Pokedex::normalize(string s) {
    // Trim leading/trailing whitespaces
    auto start = s.find_first_not_of(" \t\n\r\f\v");
    auto end = s.find_last_not_of(" \t\n\r\f\v");

    if (start == string::npos) // String is all whitespace/empty
        return "";

    s = s.substr(start, end - start + 1);

    // Lowercase
    return lower(s);
}
void Pokedex::removeFromVector(vector<int>& v, int id) {
    // Replace w/ last element, then shrink
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == id) {
            v[i] = v.back();
            v.pop_back();
            return;
        }
    }
}

/* Core Functions */
bool Pokedex::upsert(const hPokemon& p) {
    bool inserted = idMap.insert(p.id, p);
    const hPokemon* ptr = idMap.search(p.id);
    if (!ptr) return inserted; // Should be false

    // Normalize for consistency/safety
    string nm = normalize(ptr->name);
    string t1 = normalize(ptr->type1);
    string t2 = normalize(ptr->type2);

    // Update name hashmap
    auto vec = nameIdx.search(nm);
    // Separate Chaining Collision
    if (vec) {
        // If that ID isn't there (indicating a different form, then add)
        if (find(vec->begin(), vec->end(), ptr->id) == vec->end())
            vec->push_back(ptr->id);
    // Otherwise, create new one
    } else 
        nameIdx.insert(nm, vector<int>{ptr->id});

    // Update type hashmap
    if (!t1.empty()) {
        auto vec = typeIdx.search(t1);
        if (vec) {
            if (find(vec->begin(), vec->end(), ptr->id) == vec->end())
                vec->push_back(ptr->id);
        } else 
            typeIdx.insert(t1, vector<int>{ptr->id});
    }
    if (!t2.empty()) {
        auto vec = typeIdx.search(t2);
        if (vec) {
            if (find(vec->begin(), vec->end(), ptr->id) == vec->end())
                vec->push_back(ptr->id);
        } else
            typeIdx.insert(t2, vector<int>{ptr->id});
    }

    return inserted;
}
bool Pokedex::erase(int id) {
    const hPokemon* p = idMap.search(id);
    if (!p) return false;

    // Remove from secondary indexes 1st
    string nm = lower(p->name);
    if (auto vec = nameIdx.search(nm))
        removeFromVector(*vec, id);
    string t1 = lower(p->type1);
    if (auto vec = typeIdx.search(t1))
        removeFromVector(*vec, id);
    string t2 = lower(p->type2);
    if (auto vec = typeIdx.search(t2))
        removeFromVector(*vec, id);

    // Remove from main index 2nd
    return idMap.erase(id);
}

/* Queries */
const hPokemon* Pokedex::byId(int id) {
    return idMap.search(id);
}
vector<const hPokemon*> Pokedex::byName(const string& name) {
    vector<const hPokemon*> results;
    string key = lower(name);

    // Collect IDs linked to that name
    if (auto ids = nameIdx.search(key)) {
        // Save memory
        results.reserve(ids->size());
        // Find matching ID in main map
        for (int id : *ids)
            if (auto p = idMap.search(id))
                results.push_back(p);
    }
    return results;
}
vector<const hPokemon*> Pokedex::byType(const string& name) {
    vector<const hPokemon*> results;
    string key = lower(name);
    if (auto ids = typeIdx.search(key)) {
        results.reserve(ids->size());
        for (int id : *ids)
            if (auto p = idMap.search(id))
                results.push_back(p);
    }
    return results;
}

/* File Input */
bool Pokedex::loadFromCSV(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        hPokemon p;
        string cell; // Placeholder for integer values

        // Parse through each column
        // ID
        getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        p.id = stoi(cell);
        // Name
        getline(ss, p.name, ','); stripQuotes(p.name); trimWs(p.name);
        // Form - get base form, not megas
        getline(ss, p.form, ','); stripQuotes(p.form); trimWs(p.form);
        if (!p.form.empty()) {
            string dummy;
            while (getline(ss, dummy, ',')) {}
            continue;
        }
        // Type1, Type2
        getline(ss, p.type1, ','); trimWs(p.type1); stripQuotes(p.type1);
        getline(ss, p.type2, ','); trimWs(p.type2); stripQuotes(p.type2);
        // Total
        getline(ss, cell, ','); trimWs(cell); p.total = stoi(cell);
        // Stats
        getline(ss, cell, ','); trimWs(cell); p.hp = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); p.atk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); p.def = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); p.spAtk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); p.spDef = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); p.spd = stoi(cell);
        // Generation - watch for trail
        getline(ss, cell, ','); trimWs(cell);
        if (!cell.empty() && cell.back() == '\r') cell.pop_back();
        p.gen = stoi(cell);

        upsert(p);
    }
    file.close();
    return true;
}
void Pokedex::trimWs(string& s) {
    // Trailing
    while (!s.empty() && 
    (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || 
    s.back() == '\n')) s.pop_back();

    // Leading
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    if (i) s.erase(0, i);
}
void Pokedex::stripQuotes(string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
}