#ifndef _ALER_QUICLIST_H_
#define _ALER_QUICLIST_H_
#include <map>
#include <assert.h>
#include <functional>
namespace aler {
template <typename T>
class QuicList {
protected:
typedef struct ListNode {
    ListNode *lst;
    ListNode *nxt;
    T value;
    ListNode() : lst(this), nxt(this) {}
    ~ListNode() {lst = nullptr; nxt = nullptr;}
} ListNode;
    ListNode *m_fakeNode;
    std::map <T, ListNode *> m_value2ptr;
    std::function<void (const T &value)> m_onInsert;
    std::function<void (const T &value)> m_onRemove;
    //  remove node
    void removeNode(ListNode *node) {
        node->nxt->lst = node->lst;
        node->lst->nxt = node->nxt;
        if (m_onRemove) {
            m_onRemove(node->value);
        }
        m_value2ptr.erase(node->value);
        delete node;
    }
    // insert newNode next to node
    void insertNode(ListNode *node, const T &value) {
        auto newNode = new ListNode();
        auto nxtNode = node->nxt;
        newNode->lst = node;
        newNode->nxt = nxtNode;
        nxtNode->lst = newNode;
        node->nxt = newNode;
        newNode->value = value;
        if (m_onInsert) {
            m_onInsert(newNode->value);
        }
        m_value2ptr.insert(std::make_pair(value, newNode));
    }

    ListNode *head() {return m_fakeNode->nxt;}
    ListNode *tail() {return m_fakeNode->lst;}

public:
    QuicList() : m_fakeNode(new ListNode())
               , m_onInsert(nullptr)
               , m_onRemove(nullptr) {}
    ~QuicList() {reset(); delete m_fakeNode; m_fakeNode = NULL;}

    uint32_t length() {return m_value2ptr.size();}
    bool isEmpty() {return m_value2ptr.empty();}

    const T &front() {
        assert(isEmpty() == false);
        return head()->value;
    }

    const T &back() {
        assert(isEmpty() == false);
        return tail()->value;
    }

    void reset() {
        while (!isEmpty()) {
            removeNode(head());
        }
        m_value2ptr.clear();
    }

    bool pushFront(const T &value) {
        if (hasValue(value)) {
            return false;
        }
        insertNode(head()->lst, value);
        return true;
    }

    T popFront() {
        auto oldNode = head();
        T value = oldNode->value;
        removeNode(oldNode);
        return value;
    }

    bool pushBack(const T &value) {
        if (hasValue(value)) {
            return false;
        }
        insertNode(tail(), value);
        return true;
    }

    T popBack() {
        auto oldNode = tail();
        T value = oldNode->value;
        removeNode(oldNode);
        return value;
    }

    bool hasValue(const T &value) {
        return m_value2ptr.count(value);
    }

    bool remove(const T &value) {
        auto it = m_value2ptr.find(value);
        if (it == m_value2ptr.end()) {
            return false;
        }
        auto node = it->second;
        removeNode(node);
        return true;
    }

    bool insert(uint32_t pos, const T &value) {
        if (hasValue(value)) {
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
        insertNode(node, value);
        return true;
    }

    void setOnInsert(const std::function<void (const T &value)> &func) { m_onInsert = func; }
    void setOnRemove(const std::function<void (const T &value)> &func) { m_onRemove = func; }

    bool findValue(const T &value, const std::function<void (const T &value)> &func) {
        auto it = m_value2ptr.find(value);
        if (it == m_value2ptr.end()) {
            return false;
        }
        auto node = it->second;
        func(node->value);
        return true;
    }

    void forEach(const std::function<void (const T &value)> &func) {
        for (ListNode *node = head(); node != m_fakeNode; node = node->nxt) {
            func(node->value);
        }
    }
};

}

#endif 