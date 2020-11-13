#ifndef _ALER_QUICKVLIST_H_
#define _ALER_QUICKVLIST_H_
#include <map>
#include <assert.h>
#include <functional>
namespace aler {

template <typename Key, typename Value>
class QuicKVList {
protected:
typedef struct ListNode {
    ListNode *lst;
    ListNode *nxt;
    std::pair<Key, Value> kv;
    ListNode() : lst(this), nxt(this) {}
    ~ListNode() {lst = nullptr; nxt = nullptr;}
} ListNode;
    ListNode *m_fakeNode;
    std::map <Key, ListNode *> m_key2ptr;
    std::function<void (const Key &key, Value &value)> m_onInsert;
    std::function<void (const Key &key, Value &value)> m_onRemove;
    //  remove node
    void removeNode(ListNode *node) {
        node->nxt->lst = node->lst;
        node->lst->nxt = node->nxt;
        m_onRemove(node->kv.first, node->kv.second);
        m_key2ptr.erase(node->kv.first);
        delete node;
    }
    // insert newNode next to node
    void insertNode(ListNode *node, const Key &key, const Value &value) {
        auto newNode = new ListNode();
        auto nxtNode = node->nxt;
        newNode->lst = node;
        newNode->nxt = nxtNode;
        nxtNode->lst = newNode;
        node->nxt = newNode;
        newNode->kv.first = key;
        newNode->kv.second = value;
        m_onInsert(newNode->kv.first, newNode->kv.second);
        m_key2ptr.insert(std::make_pair(key, newNode));
    }

    ListNode *head() {return m_fakeNode->nxt;}
    ListNode *tail() {return m_fakeNode->lst;}

public:
    QuicKVList() : m_fakeNode(new ListNode())
               , m_onInsert([](const Key &key, Value &value){})
               , m_onRemove([](const Key &key, Value &value){}) {}
    ~QuicKVList() {reset(); delete m_fakeNode; m_fakeNode = NULL;}

    uint32_t length() {return m_key2ptr.size();}
    bool isEmpty() {return m_key2ptr.empty();}

    const std::pair<Key, Value> &front() {
        assert(isEmpty() == false);
        return head()->kv;
    }

    const std::pair<Key, Value> &back() {
        assert(isEmpty() == false);
        return tail()->kv;
    }

    void reset() {
        while (!isEmpty()) {
            removeNode(head());
        }
        m_key2ptr.clear();
    }

    bool pushFront(const Key &key, const Value &value) {
        if (hasKey(key)) {
            return false;
        }
        insertNode(head()->lst, key, value);
        return true;
    }

    std::pair<Key, Value> popFront() {
        auto oldNode = head();
        std::pair<Key, Value> kv = oldNode->kv;
        removeNode(oldNode);
        return kv;
    }

    bool pushBack(const Key &key, const Value &value) {
        if (hasKey(key)) {
            return false;
        }
        insertNode(tail(), key, value);
        return true;
    }

    std::pair<Key, Value> popBack() {
        auto oldNode = tail();
        std::pair<Key, Value> kv = oldNode->kv;
        removeNode(oldNode);
        return kv;
    }

    bool hasKey(const Key &key) {
        return m_key2ptr.count(key);
    }

    bool remove(const Key &key) {
        auto it = m_key2ptr.find(key);
        if (it == m_key2ptr.end()) {
            return false;
        }
        auto node = it->second;
        removeNode(node);
        return true;
    }

    bool insert(uint32_t pos, const Key &key, const Value &value) {
        if (hasKey(key)) {
            return false;
        }
        if (pos >= length()) {
            pos = length();
        }
        auto node = m_fakeNode;
        while (pos > 0) {
            node = node->nxt;
            pos--;
        }
        insertNode(node, key, value);
        return true;
    }

    void setOnInsert(const std::function<void (const Key &key, Value &value)> &func) { m_onInsert = func; }
    void setOnRemove(const std::function<void (const Key &key, Value &value)> &func) { m_onRemove = func; }

    bool findKey(const Key &key, const std::function<void (const Key &key, Value &value)> &func) {
        auto it = m_key2ptr.find(key);
        if (it == m_key2ptr.end()) {
            return false;
        }
        auto node = it->second;
        func(node->kv.first, node->kv.second);
        return true;
    }

    bool getValue(const Key &key, Value &value) {
        auto it = m_key2ptr.find(key);
        if (it == m_key2ptr.end()) {
            return false;
        }
        auto node = it->second;
        value = node->kv.second;
        return true;
    }
};

}

#endif 