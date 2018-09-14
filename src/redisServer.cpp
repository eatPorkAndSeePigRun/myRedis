#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "redisServer.h"
#include "wrap.h"
#include "log.h"

using namespace std;


RedisServer::RedisServer(uint16_t ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
    this->is_open = true;
    log("redisServer.cpp RedisServer(), ip: " + to_string(ip) + " port: " + to_string(port));
}

RedisServer::~RedisServer() {
    if (!this->is_open) {
        return;
    }
    this->is_open = false;
    log("redisServer.cpp ~RedisServer()");
}

bool RedisServer::init() {
    log("redisServer.cpp init()");
    this->listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (this->listenfd < 0) {
        log("***error*** redisServer.cpp init() -> socket()");
        return false;
    }
    /*// bind error: Address already in use
    int opt = 1;
    setsockopt(this->listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));*/
    bzero(&(this->servaddr), sizeof(this->servaddr));
    this->servaddr.sin_family = AF_INET;
    this->servaddr.sin_addr.s_addr = htonl(this->ip);
    this->servaddr.sin_port = htons(this->port);
    if (bind(this->listenfd, (struct sockaddr *) &this->servaddr, sizeof(this->servaddr)) < 0) {
        stringstream ss;
        ss << " fd: " << this->listenfd << " sockaddr: " << &this->servaddr << " socklen_t: " << sizeof(this->servaddr);
        log("***error*** redisServer.cpp init() -> bind()," + ss.str());
        return false;
    }
    if (listen(this->listenfd, 20) < 0) {
        log("***error*** redisServer.cpp init() -> listen(), fd: " + to_string(this->listenfd));
        return false;
    }
    FD_ZERO(&this->readfds);
    FD_ZERO(&this->writefds);
    FD_SET(this->listenfd, &this->readfds);
    return true;
}

bool RedisServer::run() {
    log("redisServer.cpp run()");
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    while (true) {
        readfds = this->readfds;
        writefds = this->writefds;
        this->maxfds = this->listenfd + this->clientfds.size() + 1;
        if (select(this->maxfds, &readfds, &writefds, nullptr, nullptr) < 0) {
            log("***error*** redisServer.cpp run() -> select(), maxfds: " + to_string(this->maxfds));
            return false;
        }
        if (FD_ISSET(this->listenfd, &readfds)) {
            if (!this->listenReadfds()) {
                continue;
            }
        }
        this->clientReadfds(readfds);
        this->clientWritefds(writefds);
    }
}

bool RedisServer::listenReadfds() {
    struct sockaddr_in cliaddr = {};
    socklen_t cliaddr_len = 0;
    int connfd = accept4(this->listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len, SOCK_NONBLOCK);
    if (connfd < 0) {
        return false;
    }
    if (FD_SETSIZE == this->maxfds) {
        return false;
    }
    FD_SET(connfd, &this->readfds);
    this->clientfds.insert(connfd);
    vector<string> temp = {};
    this->msg[connfd] = temp;
	this->requestData[connfd] = "";
    log("redisServer.cpp listenReadfds(), connfd: " + to_string(connfd));
    return true;
}

void RedisServer::clientReadfds(const fd_set &readfds) {
    for (auto &fd: this->clientfds) {
        if (FD_ISSET(fd, &readfds)) {
            // 日志
            log("redisServer.cpp clientReadfds(), fd: " + to_string(fd));
            // 处理
            char dataChar[0xffff] = {};
            switch (Read(fd, dataChar, 0xffff)) {
                case -1:
                    //this->onDisconnection(fd);
                    continue;
                    break;
                case 0:
                    this->onDisconnection(fd);
                    break;
                default:
                    this->requestData[fd] = this->requestData[fd] + string(dataChar);
					this->handleRequestData(fd, this->requestData[fd]);
                    if (!FD_ISSET(fd, &this->writefds)) {
                        FD_SET(fd, &this->writefds);
                    }
                    break;
            }
        }
    }
}

void RedisServer::clientWritefds(const fd_set &writefds) {
    for (auto &fd: this->clientfds) {
        if (FD_ISSET(fd, &writefds)) {
            // 输入检查
            if (this->msg.find(fd) == this->msg.end()) {
                if (FD_ISSET(fd, &this->writefds)) {
                    FD_CLR(fd, &this->writefds);
                }
                continue;
            }
            if (0 == this->msg[fd].size()) {
                if (FD_ISSET(fd, &this->writefds)) {
                    FD_CLR(fd, &this->writefds);
                }
                continue;
            }
            // 日志
            log("redisServer.cpp clientWritefds(), fd: " + to_string(fd));
            // 处理
            string msg;
			for (auto &item: this->msg[fd]) {
				msg = msg + item;
			}
			vector<string> ().swap(this->msg[fd]);
            char msgChar[0xffff] = {};
            strcpy(msgChar, msg.c_str());
            int res = Writen(fd, msgChar, msg.length());
            if (-1 == res) {
                this->onDisconnection(fd);
            }
            if (FD_ISSET(fd, &this->writefds)) {
                FD_CLR(fd, &this->writefds);
            }
        }
    }
}

void RedisServer::handleRequestData(int fd, string &requestData) {
    //log("redisServer.cpp handleRequestData()");
	auto p = requestData.begin();
    auto dataBegin = requestData.begin();
    auto dataEnd = requestData.end();
    string request;
    vector<string> command = {};
    while (p < dataEnd) {
        // array '*'
        if ('*' == *p) {                                   
            p++;
        } else {
            return;
        }
        // array length
        int arrayLength = 0;
        for (; *p >= '0' && *p <= '9'; p++) {
            arrayLength = arrayLength * 10 + *p - '0';
        }
        if (0 == arrayLength || '\r' != *p || '\n' != *(p + 1)) {
            return;
        } else {
            p = p + 2;
        }
        
        vector<string> ().swap(command);
        for (int i = 0; i < arrayLength; i++) {
            // '$'
            if ('$' == *p) {
                p++;
            } else {
                return;
            }
            // length
            int subsLength = 0;
            for (; *p >= '0' && *p <= '9'; p++) {
                subsLength = subsLength * 10 + *p - '0';
            }
            if (0 == subsLength) {
                return;
            }
            // "\r\n"
            if ('\r' == *p && '\n' == *(p + 1) && '\000' != *(p + 2)) {
                p = p + 2;
            } else {
                return;
            }
            // command, key, value ....
            auto begin = p;
            while ('\r' != *p && '\000' != *p) {
                p++;
            }
            if ('\000' == *p) {
                return;
            }
            auto end = p;
            command.push_back(string(begin, end));
            // "\r\n"
            if ( '\r' == *p && '\n' == *(p + 1)) {
                p = p + 2;
            } else {
                return;
            }
        }

        this->execute(request, command);
        this->msg[fd].push_back(request);
        request = "";
        requestData.erase(dataBegin, p);
        p = requestData.begin();
        dataBegin = p;
    }
}

void RedisServer::onDisconnection(int fd) {
    FD_CLR(fd, &this->readfds);
    if (FD_ISSET(fd, &this->writefds)) {
        FD_CLR(fd, &this->writefds);
    }
    close(fd);
    this->msg.erase(fd);
    //this->clientfds.erase(fd);
    log("redisServer.cpp onDisconnection(), fd: " + to_string(fd)); 
}

bool RedisServer::execute(string &data, vector<string> &command) {
    //log("redisServer.cpp execute()");
    data = "";
    string method, key, value;
    switch (command.size()) {
        case 2:
            method = command.at(0);
            key = command.at(1);
            if ("GET" == method || "get" == method) {
                if (this->db.find(key) != this->db.end()) {
					data = this->db[key];
                    data = "$" + to_string(data.length()) + "\r\n" + data + "\r\n";
                } else {
                    data = "$-1\r\n";
                }
            } else if ("DEL" == method || "del" == method) {
                if (this->db.find(key) != this->db.end()) {
                    this->db.erase(this->db.find(key));
                    data = ":1\r\n";
                } else {
                    data = ":0\r\n";
                }
            } else {
                data = "-Error message\r\n";
            }
            break;
        case 3:
            method = command.at(0);
            key = command.at(1);
            value = command.at(2);
            if ("SET" == method || "set" == method) {
                this->db[key] = value;
                data = "+OK\r\n";
            } else {
                data = "-Error message\r\n";
            }
            break;
        default:
            data = "-Error message\r\n";
    }
    return true;
}

