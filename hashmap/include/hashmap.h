#pragma once
#include "pokemon.h"

// K = key, V = value
// Key = Pokemon ID/Name/Type (to sort by)
// Value = Pokemon (represent as V b/c Pokemon is undefined here)
template<typename K, typename V>
class HashMap {
    typedef pair<K, V> entry;
    vector<vector<entry>> buckets;
    int count;
    float maxLoad;

public:
// Define within header b/c it's templated

    /* Constructor */
    HashMap(int bucketCount = 1024, float maxLoad = 0.75f) {
        if (bucketCount < 1) bucketCount = 1;
        buckets.resize(bucketCount);
        this->count = 0;
        this->maxLoad = maxLoad;
    }

    /* Helpers */
    int size() const {
        return count;
    }

    /* Core Functions */
    int hash(const K& key, int currSize) const {
        // std::hash<int> hasher;
        // auto h = hasher(key)
        size_t h = std::hash<K>{}(key);
        if (currSize <= 0) currSize = 1;
        size_t m = (size_t)(currSize);
        return (int)(h % m);
    }
    bool insert(const K& key, const V& val) {
        // Check for rehashing
        int bucketCount = buckets.size();
        if ((count + 1) > (int)(maxLoad * bucketCount))
            rehash(bucketCount * 2);

        // Find relevant bucket
        int i = hash(key, buckets.size());
        auto& bucket = buckets[i];

        // Search bucket
        for (auto& pkmn : bucket) {
            // Already exists
            if (pkmn.first == key) {
                pkmn.second = val;
                return false;
            }
        }

        // Insert
        bucket.emplace_back(key, val);
        count++;
        return true;
    }
    V* search(const K& key) {
        int i = hash(key, buckets.size());
        auto& bucket = buckets[i];

        for (auto& pkmn : bucket)
            if (pkmn.first == key)
                return &pkmn.second;

        return nullptr;
    }
    bool erase(const K& key) {
        int i = hash(key, buckets.size());
        auto& bucket = buckets[i];

        for (size_t j = 0; j < bucket.size(); j++) {
            if (bucket[j].first == key) {
                // Replace w/ last element and shrink bucket
                bucket[j] = move(bucket.back());
                bucket.pop_back();
                count--;
                return true;
            }
        }
        return false;
    }
    void rehash(int newBucketCount) {
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
};