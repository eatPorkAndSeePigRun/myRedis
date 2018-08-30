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
    int maxfds;
    map<string, string> db;
    fd_set readfds;
    fd_set writefds;
    map<int, vector<string> > msg;
    bool is_open;
    vector<int> clientfds;
public:
    RedisServer(uint16_t ip, uint16_t port);

    ~RedisServer();

    bool init();

    bool run();

    bool listenReadfds();

    void clientReadfds(const fd_set &readfds);

    void clientWritefds(const fd_set &writefds);

    bool execute(string &data);
};

#endif //REDISSERVER_H
