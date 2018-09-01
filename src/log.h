#ifndef LOG_H
#define LOG_H


#include <string>
#include <fstream>

using namespace std;


static fstream file ("server.log", fstream::in|fstream::out|fstream::app);

void log(const string &msg);

#endif  //LOG_H
