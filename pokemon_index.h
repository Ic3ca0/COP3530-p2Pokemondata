#ifndef POKEMON_INDEX_H
#define POKEMON_INDEX_H

#include <string>
#include <vector>
#include "BPlusTree.h"

struct Pokemon {
    int         id{};
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
};

class PokemonIndex {
public:
    void loadFromCSV(const std::string& path);

    const Pokemon* findById(int id) const;
    std::vector<const Pokemon*> findByName(const std::string& name) const;
    std::vector<const Pokemon*> findByType(const std::string& type) const;

    std::size_t count() const;
    bool hasRawId(int id) const;

private:
    std::vector<Pokemon> data;

    BPlusTree<int, std::size_t>         byId;
    BPlusTree<std::string, std::size_t> byName;
    BPlusTree<std::string, std::size_t> byType;
};

#endif