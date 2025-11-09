#include "pokedex.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using std::string;
using std::vector;

//https://stackoverflow.com/questions/3051486/search-a-value-for-a-given-key-in-a-hashmap

std::string Pokedex::lower(std::string s) const {
    for (std::size_t i = 0; i < s.size(); i++)
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    return s;
}

std::string Pokedex::normalize(std::string s) {
    auto start = s.find_first_not_of(" \t\n\r\f\v");
    auto end   = s.find_last_not_of(" \t\n\r\f\v");

    if (start == string::npos)
        return "";

    s = s.substr(start, end - start + 1);
    return lower(s);
}

void Pokedex::removeFromVector(vector<int>& v, int id) {
    for (std::size_t i = 0; i < v.size(); i++) {
        if (v[i] == id) {
            v[i] = v.back();
            v.pop_back();
            return;
        }
    }
}

void Pokedex::trimWs(string& s) {
    while (!s.empty() &&
           (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();

    std::size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    if (i) s.erase(0, i);
}

void Pokedex::stripQuotes(string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
}

bool Pokedex::upsert(const hPokemon& p) {
    bool inserted = idMap.insert(p.id, p);
    const hPokemon* ptr = idMap.search(p.id);
    if (!ptr) return inserted;

    string nm = normalize(ptr->name);
    string t1 = normalize(ptr->type1);
    string t2 = normalize(ptr->type2);

    auto vec = nameIdx.search(nm);
    if (vec) {
        if (std::find(vec->begin(), vec->end(), ptr->id) == vec->end())
            vec->push_back(ptr->id);
    } else {
        nameIdx.insert(nm, vector<int>{ptr->id});
    }

    if (!t1.empty()) {
        auto vec1 = typeIdx.search(t1);
        if (vec1) {
            if (std::find(vec1->begin(), vec1->end(), ptr->id) == vec1->end())
                vec1->push_back(ptr->id);
        } else {
            typeIdx.insert(t1, vector<int>{ptr->id});
        }
    }
    if (!t2.empty()) {
        auto vec2 = typeIdx.search(t2);
        if (vec2) {
            if (std::find(vec2->begin(), vec2->end(), ptr->id) == vec2->end())
                vec2->push_back(ptr->id);
        } else {
            typeIdx.insert(t2, vector<int>{ptr->id});
        }
    }

    return inserted;
}

bool Pokedex::erase(int id) {
    const hPokemon* p = idMap.search(id);
    if (!p) return false;

    string nm = lower(p->name);
    if (auto vec = nameIdx.search(nm))
        removeFromVector(*vec, id);
    string t1 = lower(p->type1);
    if (auto vec = typeIdx.search(t1))
        removeFromVector(*vec, id);
    string t2 = lower(p->type2);
    if (auto vec = typeIdx.search(t2))
        removeFromVector(*vec, id);

    return idMap.erase(id);
}

const hPokemon* Pokedex::byId(int id) {
    return idMap.search(id);
}

std::vector<const hPokemon*> Pokedex::byName(const std::string& name) {
    std::vector<const hPokemon*> results;
    string key = lower(name);

    if (auto ids = nameIdx.search(key)) {
        results.reserve(ids->size());
        for (int id : *ids)
            if (auto p = idMap.search(id))
                results.push_back(p);
    }
    return results;
}

std::vector<const hPokemon*> Pokedex::byType(const std::string& type) {
    std::vector<const hPokemon*> results;
    string key = lower(type);
    if (auto ids = typeIdx.search(key)) {
        results.reserve(ids->size());
        for (int id : *ids)
            if (auto p = idMap.search(id))
                results.push_back(p);
    }
    return results;
}

bool Pokedex::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        hPokemon p;
        string cell;

        std::getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        p.id = std::stoi(cell);

        std::getline(ss, p.name, ','); stripQuotes(p.name); trimWs(p.name);
        std::getline(ss, p.form, ','); stripQuotes(p.form); trimWs(p.form);
        if (!p.form.empty()) {
            string dummy;
            while (std::getline(ss, dummy, ',')) {}
            continue;
        }

        std::getline(ss, p.type1, ','); trimWs(p.type1); stripQuotes(p.type1);
        std::getline(ss, p.type2, ','); trimWs(p.type2); stripQuotes(p.type2);

        std::getline(ss, cell, ','); trimWs(cell); p.total = std::stoi(cell);

        std::getline(ss, cell, ','); trimWs(cell); p.hp = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); p.atk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); p.def = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); p.spAtk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); p.spDef = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); p.spd = std::stoi(cell);

        std::getline(ss, cell, ','); trimWs(cell);
        if (!cell.empty() && cell.back() == '\r') cell.pop_back();
        p.gen = std::stoi(cell);

        upsert(p);
    }

    file.close();
    return true;
}

//BuildDex

std::string BuildDex::lower(std::string s) const {
    for (std::size_t i = 0; i < s.size(); i++)
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    return s;
}

void BuildDex::trimWs(string& s) {
    while (!s.empty() &&
           (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();

    std::size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    if (i) s.erase(0, i);
}

void BuildDex::stripQuotes(string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
}

std::vector<const Build*> BuildDex::byId(int id) {
    std::vector<const Build*> results;
    auto vec = idIdx.search(id);
    if (vec) {
        results.reserve(vec->size());
        for (int i : *vec) results.push_back(&rows[static_cast<std::size_t>(i)]);
    }
    return results;
}

std::vector<const Build*> BuildDex::byName(const std::string& name) {
    std::vector<const Build*> results;
    string key = lower(name);
    auto vec = nameIdx.search(key);
    if (vec) {
        results.reserve(vec->size());
        for (int i : *vec) results.push_back(&rows[static_cast<std::size_t>(i)]);
    }
    return results;
}

bool BuildDex::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        Build b;
        string cell;

        std::getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        if (cell.empty()) continue;
        b.buildId = std::stoi(cell);

        std::getline(ss, cell, ','); trimWs(cell); stripQuotes(cell);
        b.id = std::stoi(cell);

        std::getline(ss, b.name, ','); stripQuotes(b.name); trimWs(b.name);
        std::getline(ss, b.form, ','); stripQuotes(b.form); trimWs(b.form);
        std::getline(ss, b.type1, ','); stripQuotes(b.type1); trimWs(b.type1);
        std::getline(ss, b.type2, ','); stripQuotes(b.type2); trimWs(b.type2);

        std::getline(ss, cell, ','); trimWs(cell); b.total = std::stoi(cell);

        std::getline(ss, cell, ','); trimWs(cell); b.hp = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.atk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.def = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.spAtk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.spDef = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.spd = std::stoi(cell);

        std::getline(ss, cell, ','); trimWs(cell); b.gen = std::stoi(cell);

        std::getline(ss, b.nature, ','); stripQuotes(b.nature); trimWs(b.nature);
        std::getline(ss, b.item, ','); stripQuotes(b.item); trimWs(b.item);

        std::getline(ss, cell, ','); trimWs(cell); b.f_hp = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.f_atk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.f_def = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.f_spAtk = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.f_spDef = std::stoi(cell);
        std::getline(ss, cell, ','); trimWs(cell); b.f_spd = std::stoi(cell);

        std::getline(ss, b.move1, ','); stripQuotes(b.move1); trimWs(b.move1);
        std::getline(ss, b.move2, ','); stripQuotes(b.move2); trimWs(b.move2);
        std::getline(ss, b.move3, ','); stripQuotes(b.move3); trimWs(b.move3);
        if (!std::getline(ss, b.move4)) b.move4.clear();
        else {
            stripQuotes(b.move4);
            trimWs(b.move4);
            if (!b.move4.empty() && b.move4.back() == '\r')
                b.move4.pop_back();
        }

        rows.push_back(std::move(b));
        int idx = static_cast<int>(rows.size() - 1);
        const Build& ref = rows.back();

        if (auto vec = idIdx.search(ref.id)) vec->push_back(idx);
        else idIdx.insert(ref.id, vector<int>{idx});

        string key = lower(ref.name);
        if (auto vec2 = nameIdx.search(key)) vec2->push_back(idx);
        else nameIdx.insert(key, vector<int>{idx});
    }

    file.close();
    return true;
}
