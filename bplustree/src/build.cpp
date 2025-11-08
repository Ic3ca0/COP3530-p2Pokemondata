#include "../include/build.h"
#include "../include/parse_utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

void BuildIndex::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open builds file: " + path);
    }

    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Builds file is empty: " + path);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> cols;
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            cols.push_back(cell);
        }

        if (cols.size() < 24) continue;

        PokemonBuild b;
        b.buildId   = parseInt(cols[0]);
        b.pokemonId = parseInt(cols[1]);
        if (b.pokemonId == 0 && cols[1] != "0") continue;

        b.name  = cols[2];
        b.form  = cols[3];
        b.type1 = cols[4];
        b.type2 = cols[5];

        b.total     = parseInt(cols[6]);
        b.hp        = parseInt(cols[7]);
        b.attack    = parseInt(cols[8]);
        b.defense   = parseInt(cols[9]);
        b.spAtk     = parseInt(cols[10]);
        b.spDef     = parseInt(cols[11]);
        b.speed     = parseInt(cols[12]);
        b.generation= parseInt(cols[13]);

        b.nature = cols[14];
        b.item   = cols[15];

        b.finalHP      = parseInt(cols[16]);
        b.finalAttack  = parseInt(cols[17]);
        b.finalDefense = parseInt(cols[18]);
        b.finalSpAtk   = parseInt(cols[19]);
        b.finalSpDef   = parseInt(cols[20]);
        b.finalSpeed   = parseInt(cols[21]);

        b.move1 = cols[22];
        b.move2 = cols[23];
        b.move3 = (cols.size() > 24 ? cols[24] : "");
        b.move4 = (cols.size() > 25 ? cols[25] : "");

        std::size_t idx = builds.size();
        builds.push_back(std::move(b));

        byPokemonId.insert(builds[idx].pokemonId, idx);
    }
}

std::vector<const PokemonBuild*> BuildIndex::findByPokemonId(int pokemonId) const {
    std::vector<const PokemonBuild*> out;
    auto idxs = byPokemonId.searchAll(pokemonId);
    if (idxs.empty()) return out;

    out.reserve(idxs.size());
    for (std::size_t idx : idxs) {
        out.push_back(&builds[idx]);
    }
    return out;
}
