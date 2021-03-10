#include "../src/QuicList.h" 
#include <string>
#include <iostream>
using namespace std;
using aler::QuicList;
int main() {
    QuicList <string> strList;
    strList.setOnInsert([](const std::string &str) {cout << "onInsert, value " << str << endl;});
    strList.setOnRemove([](const std::string &str) {cout << "onRemove, value " << str << endl;});
    strList.pushBack(string("1"));
    strList.pushBack(string("2"));
    strList.pushBack(string("3"));
    strList.pushBack(string("4"));
    strList.pushBack(string("5"));
    cout << "strList hasValue 3 " << strList.hasValue(string("3")) << endl;
    strList.findValue(string("3"), [](const std::string &str) {cout << "findValue " << str << endl;});
    cout << "strList hasValue 3 " << strList.hasValue(string("3")) << endl;
    strList.insert(2, string("3"));
    strList.forEach([](const std::string &str) { cout << "forEach " << str << endl; });
    while (!strList.isEmpty()) {
        auto value = strList.popFront();
        cout << "strList popFront value " << value << ", current size " << strList.length() << endl;
    }
    return 0;
}