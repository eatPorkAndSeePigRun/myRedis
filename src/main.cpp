#include <iostream>
#include <errno.h>
#include "RedisServer.h"

using namespace std;


int main() {
    RedisServer redisServer = RedisServer(INADDR_ANY, 8080);
    redisServer.open();
    redisServer.run();
    cout << "hello world" << endl;
    return 0;
}