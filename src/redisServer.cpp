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
#include "redisServer.h"
#include "wrap.h"
#include "code.h"
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
    this->clientfds.push_back(connfd);
    vector<string> temp = {};
    this->msg[connfd] = temp;
	this->tempdata[connfd] = "";
    log("redisServer.cpp listenReadfds(), connfd: " + to_string(connfd));
    return true;
}

void RedisServer::clientReadfds(const fd_set &readfds) {
    for (auto fd: this->clientfds) {
        if (FD_ISSET(fd, &readfds)) {
            // 日志
            log("redisServer.cpp clientReadfds(), fd: " + to_string(fd));
            // 处理
            char dataChar[0xffff] = {};
            switch (Read(fd, dataChar, 0xffff)) {
                case -1:
                    continue;
                    break;
                case 0:
                    FD_CLR(fd, &this->readfds);
                    if (FD_ISSET(fd, &this->writefds)) {
                        FD_CLR(fd, &this->writefds);
                    }
                    close(fd);
                    this->msg.erase(fd);
                    this->clientfds.erase(find(this->clientfds.begin(), this->clientfds.end(), fd));
                    break;
                default:
                    this->tempdata[fd] = this->tempdata[fd] + string(dataChar);
					string data;
					while (this->tempdata[fd].at(0) == '*') {
                        this->tempdata[fd].erase(0, 1);
                        size_t pos = this->tempdata[fd].find('*');
                        if (pos == string::npos) {
                            break;
                        }
                        data = "*" + this->tempdata[fd].substr(0, pos);
                        this->tempdata[fd].erase(0, pos);
                    	this->execute(data);
                    	this->msg[fd].push_back(data);
					}
                    if (!FD_ISSET(fd, &this->writefds)) {
                        FD_SET(fd, &this->writefds);
                    }
                    break;
            }
        }
    }
}

void RedisServer::clientWritefds(const fd_set &writefds) {
    for (auto fd: this->clientfds) {
        if (FD_ISSET(fd, &writefds)) {
            // 输入检查
            if (this->msg.find(fd) == this->msg.end()) {
                continue;
            }
            if (0 == this->msg[fd].size()) {
                continue;
            }
            // 日志
            log("redisServer.cpp clientWritefds(), fd: " + to_string(fd));
            // 处理
            string msg;
			for (auto item: this->msg[fd]) {
				msg = msg + item;
			}
			this->msg[fd].clear();
            char msgChar[0xffff] = {};
            strcpy(msgChar, msg.c_str());
            Writen(fd, msgChar, msg.length());
            if (FD_ISSET(fd, &this->writefds)) {
                FD_CLR(fd, &this->writefds);
            }
        }
    }
}

bool RedisServer::execute(string &data) {
    log("redisServer.cpp execute(), data: " + data);
    vector<string> command = {};
    if (!decode(command, data)) {
        data = "-Error message\r\n";
        return false;
    }
    string method, key, value;
    switch (command.size()) {
        case 2:
            method = command.at(0);
            key = command.at(1);
            if ("get" == method) {
                if (this->db.find(key) != this->db.end()) {
                    encode(data, this->db[key]);
                } else {
                    encode(data, "None");
                }
            } else if ("del" == method) {
                if (this->db.find(key) != this->db.end()) {
                    this->db.erase(this->db.find(key));
                    encode(data, 1);
                } else {
                    encode(data, 0);
                }
            } else {
                data = "-Error message\r\n";
            }
            break;
        case 3:
            method = command.at(0);
            key = command.at(1);
            value = command.at(2);
            if ("set" == method) {
                this->db[key] = value;
                encode(data, "OK");
            } else {
                data = "-Error message\r\n";
            }
            break;
        default:
            data = "-Error message\r\n";
    }
    return true;
}

