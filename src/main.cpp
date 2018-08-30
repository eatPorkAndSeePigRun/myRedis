#include <iostream>
#include "redisServer.h"

using namespace std;


int main() {
    cout << "hello world" << endl;
    RedisServer redisServer = RedisServer(INADDR_ANY, 8080);
    redisServer.init();
    if (!redisServer.run())
        redisServer.~RedisServer();
    return 0;
}
