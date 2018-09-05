#ifndef CODE_H
#define CODE_H

#include <vector>
#include <string>

using namespace std;


void encode(string &result, const string &method, bool t);

bool decode(vector<string> &result, const string &str);

#endif  //CODE_H
