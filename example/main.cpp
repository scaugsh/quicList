#include "../src/QuicList.h" 
#include <string>
#include <iostream>
using namespace std;
using aler::QuicList;
int main() {
    QuicList <string> strList;
    strList.pushBack(string("1"));
    strList.pushBack(string("2"));
    strList.pushBack(string("3"));
    strList.pushBack(string("4"));
    strList.pushBack(string("5"));
    cout << "strList hasValue 3 " << strList.hasValue(string("3")) << endl;
    strList.remove(string("3"));
    cout << "strList hasValue 3 " << strList.hasValue(string("3")) << endl;
    strList.insert(2, string("3"));
    while (!strList.isEmpty()) {
        auto value = strList.popFront();
        cout << "strList popFront value " << value << ", current size " << strList.length() << endl;
    }
    return 0;
}