#pragma once
#include <string>
#include <vector>
using namespace std;

struct hPokemon {
    int id = -1;
    string name;
    string form;
    string type1;
    string type2;
    int total = 0;
    int hp = 0;
    int atk = 0;
    int def = 0;
    int spAtk = 0;
    int spDef = 0;
    int spd = 0;
    int gen = 0;
};