#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include "util.h"
#include "code.h"

using namespace std;


void encode(string &result, const string &method, bool t) {
	if ("set" == method) {
		result = "+OK\r\n";
	} else if ("get" == method) {
		if (t) {
			result = "$" + to_string(result.length()) + "\r\n" + result + "\r\n";
		} else {
			result = "$-1\r\n";
		}
	} else if ("del" == method) {
		if (t) {
			result = ":1\r\n";
		} else {
			result = ":0\r\n";
		}
	} else {
		result = "-Error message\r\n";
	}
}

bool decode(vector<string> &result, const string &str) {
    vector<string> substrs = {};
    splitString(substrs, str, "\r\n");
    if (2 != substrs.at(0).size()){
        return false;
    }
    int number = (int) (substrs.at(0).at(1) - 48);
    if (2 == number) {
        transform(substrs.at(2).begin(), substrs.at(2).end(), substrs.at(2).begin(), ::tolower);
        string method = substrs.at(2);
        string key = substrs.at(4);
        result.push_back(method);
        result.push_back(key);
    } else if (3 == number) {
        transform(substrs.at(2).begin(), substrs.at(2).end(), substrs.at(2).begin(), ::tolower);
        string method = substrs.at(2);
        string key = substrs.at(4);
        string value = substrs.at(6);
        result.push_back(method);
        result.push_back(key);
        result.push_back(value);
    } else {
        return false;
    }
    return true;
}
