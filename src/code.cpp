#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include "util.h"
#include "code.h"

using namespace std;


vector<string> decode(string str) {
    vector<string> result;
    try {
        vector<string> strs;
        strs = splitString(str, "\r\n");
        int number = (int) (strs.at(0).at(1) - 48);
        if (2 == number) {
            transform(strs.at(2).begin(), strs.at(2).end(), strs.at(2).begin(), ::tolower);
            string method = strs.at(2);
            string key = strs.at(4);
            result.push_back(method);
            result.push_back(key);
        } else if (3 == number) {
            transform(strs.at(2).begin(), strs.at(2).end(), strs.at(2).begin(), ::tolower);
            string method = strs.at(2);
            string key = strs.at(4);
            string value = strs.at(6);
            result.push_back(method);
            result.push_back(key);
            result.push_back(value);
        }
        return result;
    } catch (...) {
        return result;
    }
}
