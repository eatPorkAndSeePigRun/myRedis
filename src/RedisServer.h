#ifndef REDISSERVER_H
#define REDISSERVER_H


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>


using namespace std;

class RedisServer{
private:
    uint32_t ip;
    uint32_t port;
    struct sockaddr_in servaddr;
    int listenfd;
    map<string, string> db;
    fd_set *readfds;
    fd_set *writefds;
    map<int, string> msg;
    bool is_open;
    bool is_close;
public:
    RedisServer();
    ~RedisServer();
    void open();
    void close();
    void run();
    void execute();
};

#endif //REDISSERVER_H

