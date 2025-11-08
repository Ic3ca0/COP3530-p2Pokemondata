#ifndef BUILD_INDEX_H
#define BUILD_INDEX_H

#include <string>
#include <vector>
#include "BPlusTree.h"

struct PokemonBuild {
    int         buildId{};
    int         pokemonId{};
    std::string name;
    std::string form;
    std::string type1;
    std::string type2;

    int total{};
    int hp{};
    int attack{};
    int defense{};
    int spAtk{};
    int spDef{};
    int speed{};
    int generation{};

    std::string nature;
    std::string item;

    int finalHP{};
    int finalAttack{};
    int finalDefense{};
    int finalSpAtk{};
    int finalSpDef{};
    int finalSpeed{};

    std::string move1;
    std::string move2;
    std::string move3;
    std::string move4;
};

class BuildIndex {
public:
    void loadFromCSV(const std::string& path);
    std::vector<const PokemonBuild*> findByPokemonId(int pokemonId) const;

private:
    std::vector<PokemonBuild> builds;
    BPlusTree<int, std::size_t> byPokemonId;
};

#endif // BUILD_INDEX_H