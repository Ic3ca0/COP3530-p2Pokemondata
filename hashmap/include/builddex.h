#pragma once
#include "hashmap.h"
#include "build.h"
#include <algorithm>
#include <fstream>
#include <sstream>

class BuildDex {
    // Main index/master storage w/ vectors
    vector<Build> rows;
    // Secondary indexes w/ hash maps
    HashMap<int, vector<int>> idIdx;
    HashMap<string, vector<int>> nameIdx;

    /* Helpers */
    string lower(string s) const;
    template <typename K>
    void idxPush(HashMap<K, vector<int>>& map, const K& key, int rowIdx);
public:
    /* Queries */
    vector<const Build*> byId(int id);
    vector<const Build*> byName(const string& name);

    /* File Input */
    bool loadFromCSV(const string& path);
    void trimWs(string& s);
    void stripQuotes(string& s);
};