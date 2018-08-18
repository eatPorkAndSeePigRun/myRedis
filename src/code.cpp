#include <stdio.h>
#include <string>
#include <typeinfo.h>
#include <vector>
#include "util.h"
#include "code.h"
using namespace std;


template<typename T>
string encode(T command){
    if(typeid(command) == typeid(int)){
        return ":"+int2str(command)+"\r\n";
    }else if(typeid(command) == typeid(string))
        return "+"+command+"\r\n";
    }
}


vector<string> decode(string str){
    vector<string> result;
    try{
        vector<string> strs;
        strs = splitString(str, "\r\n");
        int number = int2str(strs[0][1]);
        if(number == 2){
            string method = transform(strs[2].begin(), strs[2].end(), strs[2].begin(), ::tolower).c_str();  
            stirng key = strs[4];
            result.push_back(method);
            result.push_back(key);
        }else if(number == 3){
            string method = transform(strs[2].begin(), strs[2].end(), strs[2].begin(), ::tolower).c_str();  
            stirng key = strs[4];
            string value = strs[6];
            result.push_back(method);
            result.push_back(key);
            result.push_back(value);
        }
        return result;
    }catch(...){
        return result;
    }
}
