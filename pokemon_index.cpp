#include "pokemon_index.h"
#include "util.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

void PokemonIndex::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::string line;

    // Skip header
    if (!std::getline(file, line)) {
        throw std::runtime_error("File is empty: " + path);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> cols;
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            cols.push_back(cell);
        }

        if (cols.size() < 5) continue;

        Pokemon p;
        p.id   = parseInt(cols[0]);
        if (p.id == 0 && cols[0] != "0") continue;

        p.name  = cols[1];
        p.form  = cols[2];
        p.type1 = cols[3];
        p.type2 = cols[4];

        if (cols.size() > 5)  p.total      = parseInt(cols[5]);
        if (cols.size() > 6)  p.hp         = parseInt(cols[6]);
        if (cols.size() > 7)  p.attack     = parseInt(cols[7]);
        if (cols.size() > 8)  p.defense    = parseInt(cols[8]);
        if (cols.size() > 9)  p.spAtk      = parseInt(cols[9]);
        if (cols.size() > 10) p.spDef      = parseInt(cols[10]);
        if (cols.size() > 11) p.speed      = parseInt(cols[11]);
        if (cols.size() > 12) p.generation = parseInt(cols[12]);

        std::size_t idx = data.size();
        data.push_back(std::move(p));

        byId.insert(data[idx].id, idx);
        byName.insert(data[idx].name, idx);
        if (!data[idx].type1.empty()) byType.insert(data[idx].type1, idx);
        if (!data[idx].type2.empty()) byType.insert(data[idx].type2, idx);
    }
}

const Pokemon* PokemonIndex::findById(int id) const {
    std::size_t idx{};
    if (byId.search(id, idx)) {
        return &data[idx];
    }
    return nullptr;
}

std::vector<const Pokemon*> PokemonIndex::findByName(const std::string& name) const {
    std::vector<const Pokemon*> out;
    auto idxs = byName.searchAll(name);
    out.reserve(idxs.size());
    for (std::size_t i : idxs) out.push_back(&data[i]);
    return out;
}

std::vector<const Pokemon*> PokemonIndex::findByType(const std::string& type) const {
    std::vector<const Pokemon*> out;
    auto idxs = byType.searchAll(type);
    out.reserve(idxs.size());
    for (std::size_t i : idxs) out.push_back(&data[i]);
    return out;
}

std::size_t PokemonIndex::count() const {
    return data.size();
}

bool PokemonIndex::hasRawId(int id) const {
    for (const auto& p : data) {
        if (p.id == id) return true;
    }
    return false;
}