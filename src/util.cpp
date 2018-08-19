#include <string>
#include <vector>
#include <sstream>
#include "util.h"
using namespace std;


string int2str(const int &int_temp){                                
    stringstream stream;  
    stream<<int_temp;  
    return stream.str(); 
}  


int str2int(const string &string_temp)  
{  
    return atoi(string_temp.c_str());                     
}  


vector<string> splitString(const string &str,const string &pattern){
    vector<string> resVec;
    if ("" == str){
        return resVec;
    }
    string strs = str + pattern;
    size_t pos = strs.find(pattern);
    size_t size = strs.size();
    while (pos != string::npos){
        string x = strs.substr(0,pos);
        resVec.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(pattern);
    }
    return resVec;
}
