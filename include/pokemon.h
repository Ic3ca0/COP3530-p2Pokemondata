#include <string>
#include <vector>
using namespace std;

struct Pokemon {
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

class PokemonHashMap {
    typedef pair<int, Pokemon> entry;
    vector<vector<entry>> buckets;
    int count;
    float maxLoad;

public:
    /* Constructor */
    PokemonHashMap(int bucketCount = 16, float maxLoad = 0.75f);

    /* Core Functions */
    int hash(int key, int mod) const;
    bool insert(const Pokemon& p);
    Pokemon* search(int id);
    bool erase(int id);
    void rehash(int newBucketCount);

    /* Helpers */
    int size() const;
    float loadFactor() const;
};