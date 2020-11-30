#ifndef _ALER_CACHE_H_
#define _ALER_CACHE_H_
#include <map>
#include <list>
#include <vector>
#include <stdint.h>
#include <climits>
#include <QuicList.h>
#include <memory>
#include <assert.h>
#include <iostream>
namespace aler {

/*
 * 支持过期时间，过期删除需要触发一次请求，否则延迟到自然淘汰。
 * cache超过上限会优先清理最久没被访问的值
 */
template <typename Key, typename Value>
class Cache {
protected:
struct StatisticInfo {
    uint32_t getAll;
    uint32_t getSucc;
    uint32_t setAll;
    StatisticInfo() 
        : getAll(0)
        , getSucc(0)
        , setAll(0) {}
    void reset() {
        getAll = 0;
        getSucc = 0;
        setAll = 0;
    }
};
struct CacheValue {
    Value value;
    time_t expireTimeStamp;
};
    ssize_t m_maxCacheSize;
    QuicList<Key> m_keys;
    std::map<Key, CacheValue> m_datas;
    StatisticInfo m_statisticInfo;
protected:
    void checkTimeoutNode() {
        time_t now = time(NULL);
        while (!m_keys.isEmpty()) {
            const Key &key = m_keys.front();
            auto it = m_datas.find(key);
            assert(it != m_datas.end());
            auto &value = it->second;
            if (value.expireTimeStamp < now) {
                m_keys.popFront();
                m_datas.erase(it);
            } else {
                std::cout << "key " << key << ", expireTimeStamp " << value.expireTimeStamp << ", now " << now << std::endl;
                break;
            }
        }
    }

    void removeOldValue(uint32_t count = 1) {
        while (count > 0) {
            count--;
            const Key &key = m_keys.front();
            auto it = m_datas.find(key);
            assert(it != m_datas.end());
            // auto &value = it->second;
            m_keys.popFront();
            m_datas.erase(it);
        }
    }

    void updateKey(const Key &key) {
        m_keys.remove(key);
        m_keys.pushBack(key);
    }

public:
    void setMaxCacheSize(ssize_t size) { m_maxCacheSize = size; }

    size_t size() { return m_datas.size(); }
    Cache() {}
    Cache(ssize_t maxCacheSize = 100000) : m_maxCacheSize(maxCacheSize) {}
    virtual ~Cache() { clear(); }
    void Timer() { checkTimeoutNode(); }
    bool set(const Key &key, const Value &value, int32_t ttl = -1) {
        m_statisticInfo.setAll++;
        if (m_datas.size() >= m_maxCacheSize) {
            removeOldValue(m_datas.size() - m_maxCacheSize + 1);
        }
        CacheValue cacheValue;
        cacheValue.value = value;
        cacheValue.expireTimeStamp = (ttl == -1) ? LONG_MAX : (time(NULL) + ttl);
        if (m_keys.hasValue(key)) {
            m_keys.remove(key);
        }
        m_keys.pushBack(key);
        m_datas[key] = cacheValue;
        return true;
    }

    bool get(const Key &key, Value *valuePtr = nullptr, int32_t *ttlPtr = nullptr) {
        m_statisticInfo.getAll++;
        if (m_keys.hasValue(key) == false) {
            return false;
        }
        
        auto it = m_datas.find(key);
        assert(it != m_datas.end());
        auto &cacheValue = it->second;
        time_t now = time(NULL);
        if (cacheValue.expireTimeStamp < now) {
            m_keys.remove(key);
            m_datas.erase(it);
            return false;
        }
        m_statisticInfo.getSucc++;

        updateKey(key);

        if (valuePtr) {
            *valuePtr = cacheValue.value;
        }
        if (ttlPtr) {
            if (cacheValue.expireTimeStamp == LONG_MAX) {
                *ttlPtr = -1;
            } else {
                *ttlPtr = ((cacheValue.expireTimeStamp >= now) ? (cacheValue.expireTimeStamp - now) : 0);
            }
        }
        return true;
    }

    void clear() {
        m_keys.reset();
        m_datas.clear();
        m_statisticInfo.reset();
    }

    void resetStatistic() {
        m_statisticInfo.reset();
    }

    uint32_t statisticInfoGetAll() {
        return m_statisticInfo.getAll;
    }

    uint32_t statisticInfoGetSucc() {
        return m_statisticInfo.getSucc;
    }

    uint32_t statisticInfoSetAll() {
        return m_statisticInfo.setAll;
    }
};

}
#endif 