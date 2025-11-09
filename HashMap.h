#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include <utility>
#include <functional>

//https://www.geeksforgeeks.org/cpp/how-to-use-hashmap-in-cpp/
template<typename K, typename V>
class HashMap {
    using entry = std::pair<K, V>;
    std::vector<std::vector<entry>> buckets;
    int   count;
    float maxLoad;

public:
    HashMap(int bucketCount = 1024, float maxLoad = 0.75f) {
        if (bucketCount < 1) bucketCount = 1;
        buckets.resize(bucketCount);
        this->count   = 0;
        this->maxLoad = maxLoad;
    }

    int size() const {
        return count;
    }

    int hash(const K& key, int currSize) const {
        std::size_t h = std::hash<K>{}(key);
        if (currSize <= 0) currSize = 1;
        std::size_t m = static_cast<std::size_t>(currSize);
        return static_cast<int>(h % m);
    }

    bool insert(const K& key, const V& val) {
        int bucketCount = static_cast<int>(buckets.size());
        if ((count + 1) > static_cast<int>(maxLoad * bucketCount))
            rehash(bucketCount * 2);

        int i = hash(key, static_cast<int>(buckets.size()));
        auto& bucket = buckets[i];

        for (auto& e : bucket) {
            if (e.first == key) {
                e.second = val;
                return false;
            }
        }

        bucket.emplace_back(key, val);
        count++;
        return true;
    }

    V* search(const K& key) {
        int i = hash(key, static_cast<int>(buckets.size()));
        auto& bucket = buckets[i];

        for (auto& e : bucket)
            if (e.first == key)
                return &e.second;

        return nullptr;
    }

    bool erase(const K& key) {
        int i = hash(key, static_cast<int>(buckets.size()));
        auto& bucket = buckets[i];

        for (std::size_t j = 0; j < bucket.size(); j++) {
            if (bucket[j].first == key) {
                bucket[j] = std::move(bucket.back());
                bucket.pop_back();
                count--;
                return true;
            }
        }
        return false;
    }

    // https://stackoverflow.com/questions/3578083/what-is-the-best-way-to-use-a-hashmap-in-c
    void rehash(int newBucketCount) {
        if (newBucketCount < 1) newBucketCount = 1;
        std::vector<std::vector<entry>> newBuckets(newBucketCount);

        for (auto& bucket : buckets) {
            for (auto& e : bucket) {
                int i = hash(e.first, newBucketCount);
                newBuckets[i].push_back(std::move(e));
            }
        }
        buckets = std::move(newBuckets);
    }
};

#endif
