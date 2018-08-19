#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
using namespace std;


string int2str(const int &int_temp);

int str2int(const string &string_temp);

vector<string> splitString(const string &str, const string &pattern);

#endif //UTIL_H
