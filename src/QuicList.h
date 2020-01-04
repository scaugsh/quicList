#ifndef _ALER_QUICLIST_H_
#define _ALER_QUICLIST_H_
#include <map>
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
    //  remove node
    void removeNode(ListNode *node) {
        node->nxt->lst = node->lst;
        node->lst->nxt = node->nxt;
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
        m_value2ptr.insert(std::make_pair(value, newNode));
    }

public:
    QuicList() : m_fakeNode(new ListNode()) {}
    ~QuicList() {delete m_fakeNode; m_fakeNode = NULL;}

    uint32_t length() {return m_value2ptr.size();}
    bool isEmpty() {return m_value2ptr.empty();}

    ListNode *head() {return m_fakeNode->nxt;}
    ListNode *tail() {return m_fakeNode->lst;}

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
};
}

#endif 