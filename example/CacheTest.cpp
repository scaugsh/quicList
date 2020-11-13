#include <time.h>
#include <iostream>
#include <unistd.h>
#include <set>
#include "Cache.h"
using namespace std;
#define HOLD while (true) { sleep(1); }
template <typename Key, typename Value>
class TestCache {
protected:
    aler::Cache<Key, Value> cache;
public:
    TestCache() : cache(10) {}
    TestCache(ssize_t sz) : cache(sz) {}
    void set(const Key &key, const Value &value, int32_t ttl = -1) {
        cache.set(key, value, ttl);
        cout << "set key " << key << ", value " << value << ", ttl " << ttl << ", now " << time(NULL) << endl;
    }
    bool get(const Key &key, Value *value = nullptr, int32_t *ttl = nullptr) {
        Value tempValue;
        int32_t tempTtl;
        if (value == nullptr) {
            value = &tempValue;
        }
        if (ttl == nullptr) {
            ttl = &tempTtl;
        }

        if (cache.get(key, value, ttl)) {
            cout << "find key " << key << ", value " << *value << ", ttl " << *ttl << ", now " << time(NULL) << endl;
            return true;
        } else {
            cout << "can't find key " << key << ", now " << time(NULL) << endl;
            return false;
        }
        return false;
    }

    ssize_t size() { return cache.size(); }
};

void sample() {
    TestCache<uint32_t, std::string> cache;
    cache.set(1, "one", 1);
    cache.set(10, "ten", 10);
    sleep(5);
    cache.get(1);
    cache.get(10);
}

void stressTest(uint32_t testCount = 1000000) {
    // TestCache<uint64_t, std::string> cache(testCount + 1);
    aler::Cache<uint64_t, std::string> cache(testCount + 1);
    std::map<uint64_t, time_t> testMap;
    for (uint32_t i = 0; i < testCount; i++) {
        uint64_t randNumber = ((uint64_t)rand() << 32) | rand();
        std::string randNumberStr = std::to_string(randNumber);
        uint32_t ttl = rand() % 60 + 1;
        cache.set(randNumber, randNumberStr, ttl);
        if (testMap.count(randNumber)) {
            cout << "WARN!!! number " << randNumber << " has inserted, oldExpireTimeStamp " << testMap[randNumber] << ", now " << time(NULL) << endl;
        }
        testMap[randNumber] = time(NULL) + ttl;
    }

    cout << "insert succ! cache current size " << cache.size() << endl;

    uint32_t idx = 0;
    for (auto &number2ttl: testMap) {
        auto &number = number2ttl.first;
        auto &expireTimeStamp = number2ttl.second;
        idx++;
        if (idx % (testCount / 60 + 1) == 0) {
            sleep(1);
            cache.Timer();
            idx = 0;
        }
        uint32_t now = time(NULL);
        std::string randNumberStr;
        int32_t ttl;
        bool exist = cache.get(number, &randNumberStr, &ttl);
        if (!exist) {
            if (expireTimeStamp > now) {
                cout << "ERROR!!! key " << number << ", now " << now << ", expireTimeStamp " << expireTimeStamp << endl;
            }
        } else {
            if (randNumberStr != std::to_string(number)) {
                cout << "ERROR!!! key " << number << ", value " << randNumberStr << endl;
            }
            if (expireTimeStamp < now) {
                cout << "ERROR!!! key " << number << ", now " << now << ", expireTimeStamp " << expireTimeStamp << ", ttl " << ttl << endl;
            }
        }
    }

    cout << "finish stressTest!" << endl;
}

std::string gen_random(const int len) {
    std::string str;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        str += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return str;
}

void memorytest(uint32_t testCount) {
    aler::Cache<std::string, uint32_t> cache(testCount);
    for (uint32_t i = 0; i < testCount; i++) {
        cache.set(gen_random(32), -1);
    }

    cout << "insert succ! cache current size " << cache.size() << endl;

    HOLD;
}

void mapMemoryTest(uint32_t testCount) {
    map<std::string, uint32_t> m;
    for (uint32_t i = 0; i < testCount; i++) {
        m[gen_random(32)] = -1;
    }

    cout << "insert succ! map current size " << m.size() << endl;

    HOLD;
}

void memoryTestV2() {
    aler::Cache<std::string, uint32_t> cache(10000000);
    
    while (true) {
        uint32_t cnt = 1000000;
        uint32_t ttl = rand() % 60 + 1;
        for (uint32_t i = 0; i < cnt; i++) {
            cache.set(gen_random(32), i, ttl);
        }
        size_t oldSize = cache.size();
        cache.Timer();
        cout << "INFO!!! cache size " << oldSize << " -> " << cache.size() << endl;
    }
}

int main() {
    srand(time(NULL));
    // for (uint32_t i = 0; i < 100; i++) {
    //     stressTest(10000000);
    // }
    // memorytest(100000);
    memoryTestV2();
    HOLD;
    return 0;
}