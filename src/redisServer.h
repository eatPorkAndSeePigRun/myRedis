#ifndef REDISSERVER_H
#define REDISSERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;


class RedisServer {
private:
    uint16_t ip;
    uint16_t port;
    struct sockaddr_in servaddr;
    int listenfd;
    int maxfds;
    bool is_open;
    set<int> clientfds;
    fd_set readfds;
    fd_set writefds;
    map<int, string> readMsg;
	map<int, string> writeMsg;
    map<string, string> db;

    void listenReadfds(const fd_set &readfds);

    void clientReadfds(const fd_set &readfds);

    void clientWritefds(const fd_set &writefds);

	void handleRequestData(int fd, string &requestData);

    void onDisconnection(int fd);

    bool execute(string &data, vector<string> &command);

public:
    RedisServer(uint16_t ip, uint16_t port);

    ~RedisServer();

    bool init();

    bool run();

};

#endif //REDISSERVER_H
