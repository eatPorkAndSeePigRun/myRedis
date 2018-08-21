#include <iostream>
#include <errno.h>
#include "RedisServer.h"

using namespace std;


int main() {
    cout << "hello world" << endl;
    RedisServer redisServer = RedisServer(INADDR_ANY, 8080);
    redisServer.open();
    redisServer.run();
    return 0;
}
