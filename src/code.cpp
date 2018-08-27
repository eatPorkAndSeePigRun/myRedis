#include <algorithm>
#include <stdio.h>
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
        int number = (int) (strs[0][1] - 48);
        if (number == 2) {
            transform(strs[2].begin(), strs[2].end(), strs[2].begin(), ::tolower);
            string method = strs[2];
            string key = strs[4];
            result.push_back(method);
            result.push_back(key);
        } else if (number == 3) {
            transform(strs[2].begin(), strs[2].end(), strs[2].begin(), ::tolower);
            string method = strs[2];
            string key = strs[4];
            string value = strs[6];
            result.push_back(method);
            result.push_back(key);
            result.push_back(value);
        }
        return result;
    } catch (...) {
        return result;
    }
}
