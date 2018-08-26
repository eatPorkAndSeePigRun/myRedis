#ifndef CODE_H
#define CODE_H

#include <typeinfo>
#include <sstream>
#include <vector>
#include <string>


using namespace std;

template<class T>
string encode(T command) {
    stringstream ss;
    ss << command;
    if (typeid(command) == typeid(int)) {
        return ":" + ss.str() + "\\r\\n";
    } else if (typeid(command) == typeid(string)) {
        return "+" + ss.str() + "\\r\\n";
    } else if (typeid(command) == typeid(const char *)) {
        return "+" + ss.str() + "\\r\\n";
    }
    return "-Error message\\r\\n";
}

vector<string> decode(string str);

#endif  //CODE_H
