#include <cstdio>
#include <ctime>
#include <string>
#include "log.h"

using namespace std;


void log(string msg) {
    time_t curtime;
    time(&curtime);
    FILE *file = fopen("server.log", "a+");
    fprintf(file, "%s\t%s\r\n", msg.c_str(), ctime(&curtime));
    fclose(file);
}
