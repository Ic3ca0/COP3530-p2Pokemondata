#include "../include/builddex.h"

/* Helpers */
string BuildDex::lower(string s) const {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = tolower((unsigned char)s[i]);
    return s;
}
template <typename K>
void BuildDex::idxPush(HashMap<K, vector<int>>& map, const K& key, int rowIdx) {
    auto vec = map.search(key);
    if (vec) vec->push_back(rowIdx);
    else map.insert(key, vector<int>{rowIdx});
}

/* Queries */
vector<const Build*> BuildDex::byId(int id) {
    vector<const Build*> results;
    auto vec = idIdx.search(id);
    if (vec) {
        results.reserve(vec->size());
        for (int i : *vec) results.push_back(&rows[i]);
    }
    return results;
}
vector<const Build*> BuildDex::byName(const string& name) {
    vector<const Build*> results;
    string key = lower(name);
    auto vec = nameIdx.search(key);
    if (vec) {
        results.reserve(vec->size());
        for (int i : *vec) results.push_back(&rows[i]);
    }
    return results;
}

/* File Input */
bool BuildDex::loadFromCSV(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Build b;
        string cell;

        // Build ID
        getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        if (cell.empty()) continue;
        b.buildId = stoi(cell);
        // ID
        getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        b.id = stoi(cell);
        // Name, Form, Type1, Type2
        getline(ss, b.name, ','); stripQuotes(b.name); trimWs(b.name);
        getline(ss, b.form, ','); stripQuotes(b.form); trimWs(b.form);
        getline(ss, b.type1, ','); stripQuotes(b.type1); trimWs(b.type1);
        getline(ss, b.type2, ','); stripQuotes(b.type2); trimWs(b.type2);
        // Total
        getline(ss, cell, ','); trimWs(cell); b.total = stoi(cell);
        // Base Stats
        getline(ss, cell, ','); trimWs(cell); b.hp = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.atk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.def = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.spAtk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.spDef = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.spd = stoi(cell);
        // Generation
        getline(ss, cell, ','); trimWs(cell); b.gen = stoi(cell);
        // Nature, Item
        getline(ss, b.nature, ','); stripQuotes(b.nature); trimWs(b.nature);
        getline(ss, b.item, ','); stripQuotes(b.item); trimWs(b.item);
        // Final stats
        getline(ss, cell, ','); trimWs(cell); b.f_hp = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.f_atk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.f_def = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.f_spAtk = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.f_spDef = stoi(cell);
        getline(ss, cell, ','); trimWs(cell); b.f_spd = stoi(cell);
        // Moves
        getline(ss, b.move1, ','); stripQuotes(b.move1); trimWs(b.move1);
        getline(ss, b.move2, ','); stripQuotes(b.move2); trimWs(b.move2);
        getline(ss, b.move3, ','); stripQuotes(b.move3); trimWs(b.move3);
        if (!getline(ss, b.move4)) b.move4.clear();
        else {
            stripQuotes(b.move4); 
            trimWs(b.move4); 
            if (!b.move4.empty() && b.move4.back() == '\r')
                b.move4.pop_back();
        }

        // Store into row
        rows.push_back(move(b));
        int idx = (int)(rows.size() - 1);
        // Create reference to newly added build
        const Build& ref = rows.back();

        // Update idIdx
        if (auto vec = idIdx.search(ref.id)) vec->push_back(idx);
        else idIdx.insert(ref.id, vector<int>{idx});

        // Update nameIdx
        string key = lower(ref.name);
        if (auto vec2 = nameIdx.search(key)) vec2->push_back(idx);
        else nameIdx.insert(move(key), vector<int>{idx});
    }

    file.close();
    return true;
}
void BuildDex::trimWs(string& s) {
    // Trailing
    while (!s.empty() && 
    (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || 
    s.back() == '\n')) s.pop_back();

    // Leading
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    if (i) s.erase(0, i);
}
void BuildDex::stripQuotes(string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
}