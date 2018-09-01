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
    file<<time<<"    "<<msg<<endl;
}
