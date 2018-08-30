#include <fstream>
#include <string>
#include <ctime>
#include "log.h"

using namespace std;


void log(const string &msg) {
    if ("" == msg){
        return;
    }
    time_t curtime;
    time(&curtime);
    string time = ctime(&curtime);
    time.pop_back();
    fstream file;
    file.open("server.log", fstream::in|fstream::out|fstream::app);
    file<<time<<"    "<<msg<<endl;
    file.close(); 
    /*
    time_t curtime;
    time(&curtime);
    FILE *file = fopen("server.log", "a+");
    fprintf(file, "%s\t%s\r\n", msg.c_str(), ctime(&curtime));
    fclose(file);
    */
}
