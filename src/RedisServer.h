#ifndef REDISSERVER_H
#define REDISSERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>


using namespace std;


class RedisServer {
private:
    uint16_t ip;
    uint16_t port;
    struct sockaddr_in servaddr;
    int listenfd;
    map<string, string> db;
    fd_set readfds;
    fd_set writefds;
    map<int, vector<string> > msg;
    bool is_open;
    bool is_close;
    vector<int> clientfds;
public:
    RedisServer(uint16_t ip, uint16_t port);

    ~RedisServer();

    void open();

    void close();

    void run();

    string execute(string data);
};

#endif //REDISSERVER_H
