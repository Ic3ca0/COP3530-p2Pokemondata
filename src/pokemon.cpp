#include "../include/pokemon.h"

/* Constructor */
PokemonHashMap::PokemonHashMap(int bucketCount, float maxLoad) {
    buckets.resize(bucketCount);
    this->buckets[bucketCount];
    this->count = 0;
    this->maxLoad = maxLoad;
}

/* Core Functions */
int PokemonHashMap::hash(int key, int mod) const {
    // std::hash<int> hasher;
    // auto h = hasher(key);
    size_t h = std::hash<int>{}(key);

    // Modulus (bucketCount - possible indexes)
    if (mod <= 0) mod = 1;
    size_t m = (size_t)(mod);
    
    return (int)(h % m);
}
bool PokemonHashMap::insert(const Pokemon& p) {
    // Check for rehashing
    int bucketCount = buckets.size();
    if ((count + 1) > (int)(maxLoad * bucketCount))
        rehash(bucketCount * 2);

    // Insert
    int i = hash(p.id, buckets.size());
    vector<entry>& bucket = buckets[i];

    // Handle dupes by updating stats w/ new one's
    for (auto& pkmn : bucket) {
        if (pkmn.first == p.id) {
            pkmn.second = p;
            return false;
        }
    }

    bucket.emplace_back(p.id, p);
    count++;
    return true;
}
Pokemon* PokemonHashMap::search(int id) {
    // Grab bucket
    int i = hash(id, buckets.size());
    vector<entry>& bucket = buckets[i];

    // Search within bucket
    for (auto& pkmn : bucket) 
        if (pkmn.first == id) return &pkmn.second;

    return nullptr;
}
bool PokemonHashMap::erase(int id) {
    // Grab bucket
    int idx = hash(id, buckets.size());
    vector<entry>& bucket = buckets[idx];

    // Search through bucket
    for (size_t i = 0; i < bucket.size(); i++) {
        if (bucket[i].first == id) {
            // Replace w/ last element and shrink bucket
            bucket[i] = move(bucket.back());
            bucket.pop_back();
            count--;
            return true;
        }
    }
    return false;
}
void PokemonHashMap::rehash(int newBucketCount) {
    if (newBucketCount < 1) newBucketCount = 1;
    vector<vector<entry>> newBuckets(newBucketCount);

    int i = 0;
    for (auto& bucket : buckets) {
        for (auto& pkmn : bucket) {
            i = hash(pkmn.first, newBucketCount);
            newBuckets[i].push_back(move(pkmn));
        }
    }

    buckets = move(newBuckets);
}

/* Helpers */
int PokemonHashMap::size() const {
    return count;
}