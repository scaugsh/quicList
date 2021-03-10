#include "../src/QuicKVList.h" 
#include <string>
#include <iostream>
using namespace std;
using aler::QuicKVList;
int main() {
    QuicKVList <std::string, std::string> strList;
    strList.setOnInsert([](const std::string &key, std::string &value) {cout << "onInsert, key " << key << " value " << value << endl;});
    strList.setOnRemove([](const std::string &key, std::string &value) {cout << "onRemove, key " << key << " value " << value << endl;});
    strList.pushBack(string("1"), string("one"));
    strList.pushBack(string("2"), string("two"));
    strList.pushBack(string("3"), string("three"));
    strList.pushBack(string("4"), string("four"));
    strList.pushBack(string("5"), string("five"));
    cout << "strList hasKey 3 " << strList.hasKey(string("3")) << endl;
    strList.findKey(string("3"), [](const std::string &key, std::string &value) {cout << "findKey " << key << ", value " << value << ", change value to " << "three!!" << endl; value = "three!!";});
    strList.findKey(string("3"), [](const std::string &key, std::string &value) {cout << "findKey " << key << ", value " << value << endl;});
    cout << "strList hasKey 3 " << strList.hasKey(string("3")) << endl;
    strList.insert(2, string("3"), string("three"));

    strList.forEach([](const std::string &key, std::string &value) { cout << "forEach key " << key << " value " << value << endl; value = "hahaha";});
    while (!strList.isEmpty()) {
        auto kv = strList.popFront();
        cout << "strList popFront key " << kv.first << ", value " << kv.second << ", current size " << strList.length() << endl;
    }
    return 0;
}