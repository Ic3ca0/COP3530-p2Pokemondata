#ifndef POKEDEX_H
#define POKEDEX_H

#include <string>
#include <vector>
#include "HashMap.h"

struct hPokemon {
    int id = -1;
    std::string name;
    std::string form;
    std::string type1;
    std::string type2;
    int total = 0;
    int hp = 0;
    int atk = 0;
    int def = 0;
    int spAtk = 0;
    int spDef = 0;
    int spd = 0;
    int gen = 0;
};

struct Build {
    int buildId = 0;
    int id = 0;
    std::string name;
    std::string form;
    std::string type1;
    std::string type2;
    int total = 0;
    int hp = 0;
    int atk = 0;
    int def = 0;
    int spAtk = 0;
    int spDef = 0;
    int spd = 0;
    int gen = 0;
    std::string nature;
    std::string item;
    int f_hp = 0;
    int f_atk = 0;
    int f_def = 0;
    int f_spAtk = 0;
    int f_spDef = 0;
    int f_spd = 0;
    std::string move1;
    std::string move2;
    std::string move3;
    std::string move4;
};

class Pokedex {
    HashMap<int, hPokemon> idMap;
    HashMap<std::string, std::vector<int>> nameIdx;
    HashMap<std::string, std::vector<int>> typeIdx;

    std::string lower(std::string s) const;
    std::string normalize(std::string s);
    void removeFromVector(std::vector<int>& v, int id);
    void trimWs(std::string& s);
    void stripQuotes(std::string& s);

public:
    bool upsert(const hPokemon& p);
    bool erase(int id);

    const hPokemon* byId(int id);
    std::vector<const hPokemon*> byName(const std::string& name);
    std::vector<const hPokemon*> byType(const std::string& type);

    bool loadFromCSV(const std::string& path);
};

class BuildDex {
    std::vector<Build> rows;
    HashMap<int, std::vector<int>>    idIdx;
    HashMap<std::string, std::vector<int>> nameIdx;

    std::string lower(std::string s) const;
    void trimWs(std::string& s);
    void stripQuotes(std::string& s);

public:
    std::vector<const Build*> byId(int id);
    std::vector<const Build*> byName(const std::string& name);
    bool loadFromCSV(const std::string& path);
};

#endif