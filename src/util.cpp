#include <string>
#include <vector>
#include "util.h"

using namespace std;


bool splitString(vector<string> &resVec, const string &str, const string &pattern) {
    if ("" == str) {
        return false;
    }
    resVec.clear();
    string strs = str + pattern;
    size_t pos = strs.find(pattern);
    size_t strSize = strs.size();
    size_t patternSize = pattern.size();
    while (pos != string::npos) {
        string x = strs.substr(0, pos);
        resVec.push_back(x);
        strs = strs.substr(pos + patternSize, strSize);
        pos = strs.find(pattern);
    }
    return true;
}
