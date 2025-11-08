#pragma once

#include <string>
#include <vector>
#include "bplustree.h"

struct bPokemon {
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

    const bPokemon* findById(int id) const;
    std::vector<const bPokemon*> findByName(const std::string& name) const;
    std::vector<const bPokemon*> findByType(const std::string& type) const;

    std::size_t count() const;
    bool hasRawId(int id) const;

private:
    std::vector<bPokemon> data;

    BPlusTree<int, std::size_t>         byId;
    BPlusTree<std::string, std::size_t> byName;
    BPlusTree<std::string, std::size_t> byType;
};
