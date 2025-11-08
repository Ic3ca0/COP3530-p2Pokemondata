#pragma once
#include <string>
#include <vector>
using namespace std;

struct Build {
    int buildId = 0;
    int id = 0;
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
    string nature;
    string item;
    int f_hp = 0;
    int f_atk = 0;
    int f_def = 0;
    int f_spAtk = 0;
    int f_spDef = 0;
    int f_spd = 0;
    string move1;
    string move2;
    string move3;
    string move4;
};