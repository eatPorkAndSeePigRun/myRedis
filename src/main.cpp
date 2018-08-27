#include <iostream>
#include "RedisServer.h"

using namespace std;


int main() {
    cout << "hello world" << endl;
    RedisServer redisServer = RedisServer(INADDR_ANY, 8080);
    redisServer.open();
    redisServer.run();
    redisServer.close();
    return 0;
}
