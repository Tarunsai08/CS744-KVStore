#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <mutex>
#include <memory>

using namespace std;

class LRUCache {
private:
    int capacity;
    unordered_map<int, pair<shared_ptr<string>, list<int>::iterator>> cache;
    list<int> lru;
    mutex mtx;

public:
    LRUCache(int cap) : capacity(cap) {}

    void put(int key, const string &value) {
        lock_guard<mutex> lock(mtx);
        auto sp = make_shared<string>(value);

        if (cache.count(key)) {
            lru.erase(cache[key].second);
            lru.push_front(key);
            cache[key] = {sp, lru.begin()};
            return;
        }

        if (cache.size() >= capacity) {
            int old = lru.back();
            lru.pop_back();
            cache.erase(old);
        }

        lru.push_front(key);
        cache[key] = {sp, lru.begin()};
    }

    string get(int key) {
        lock_guard<mutex> lock(mtx);
        if (!cache.count(key)) return "";

        lru.erase(cache[key].second);
        lru.push_front(key);
        cache[key].second = lru.begin();

        return *(cache[key].first);
    }

    bool del(int key) {
        lock_guard<mutex> lock(mtx);
        if (!cache.count(key)) return false;

        lru.erase(cache[key].second);
        cache.erase(key);
        return true;
    }
};

#endif