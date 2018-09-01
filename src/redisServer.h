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
	map<int, string> tempdata;
    bool is_open;
    vector<int> clientfds;

    bool listenReadfds();

    void clientReadfds(const fd_set &readfds);

    void clientWritefds(const fd_set &writefds);

    bool execute(string &data);

public:
    RedisServer(uint16_t ip, uint16_t port);

    ~RedisServer();

    bool init();

    bool run();

};

#endif //REDISSERVER_H
