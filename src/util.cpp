#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include "util.h"

using namespace std;


vector<string> splitString(const string &str, const string &pattern) {
    vector<string> resVec;
    if ("" == str) {
        return resVec;
    }
    string strs = str + pattern;
    size_t pos = strs.find(pattern);
    size_t size = strs.size();
    while (pos != string::npos) {
        string x = strs.substr(0, pos);
        resVec.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(pattern);
    }
    return resVec;
}