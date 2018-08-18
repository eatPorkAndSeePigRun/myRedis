#ifndef CODE_H
#define CODE_H

#include <vector>
#include <string>
using namespace std;


template<typename T>
string encode(T command);

vector<string> decode(string str);

#endif  //CODE_H
