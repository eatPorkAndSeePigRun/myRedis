#ifndef CODE_H
#define CODE_H

#include <typeinfo>
#include <sstream>
#include <vector>
#include <string>

using namespace std;


template<class T>
void encode(string &result, T command) {
    stringstream ss;
    ss << command;
    if (typeid(command) == typeid(int)) {
        result = ":" + ss.str() + "\r\n";
    } else if (typeid(command) == typeid(string)) {
        result = "+" + ss.str() + "\r\n";
    } else if (typeid(command) == typeid(const char *)) {
        result = "+" + ss.str() + "\r\n";
    } else {
        result = "-Error message\r\n";
    }
}

bool decode(vector<string> &result, const string &str);

#endif  //CODE_H
