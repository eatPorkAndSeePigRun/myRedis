#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <vector>
#include <string>
#include <map>
#include "RedisServer.h"
#include "wrap.h"
#include "code.h"
#include "log.h"

using namespace std;


RedisServer::RedisServer(uint16_t ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
    this->is_open = true;
    this->is_close = true;
    log("RedisServer RedisServer(), " + to_string(ip) + " " + to_string(port));
}


RedisServer::~RedisServer() {
    this->close();
    log("RedisServer ~RedisServer()");
}


void RedisServer::open() {
    if (!this->is_close) {
        return;
    }
    log("RedisServer open()");
    this->is_close = false;
    this->listenfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK);
    bzero(&(this->servaddr), sizeof(this->servaddr));
    this->servaddr.sin_family = AF_INET;
    this->servaddr.sin_addr.s_addr = htonl(this->ip);
    this->servaddr.sin_port = htons(this->port);
    Bind(this->listenfd, (struct sockaddr *) &(this->servaddr), sizeof(this->servaddr));
    Listen(this->listenfd, 20);
    FD_ZERO(&this->readfds);
    FD_ZERO(&this->writefds);
    FD_SET(this->listenfd, &this->readfds);
}


void RedisServer::close() {
    if (!this->is_open) {
        return;
    }
    log("RedisServer close()");
    this->is_open = false;
    for (auto fd: this->clientfds) {
        Close(fd);
    }
    Close(this->listenfd);
}


void RedisServer::run() {
    log("RedisServer run()");
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    while (true) {
        readfds = this->readfds;
        writefds = this->writefds;
        int maxfds = this->listenfd + this->clientfds.size() + 1;
        Select(maxfds, &readfds, &writefds, nullptr, nullptr);
        if (FD_ISSET(this->listenfd, &readfds)) {
            struct sockaddr_in cliaddr = {};
            socklen_t cliaddr_len;
            int connfd = Accept4(this->listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len, SOCK_NONBLOCK);
            if (0 == connfd) {
                continue;
            }
            FD_SET(connfd, &this->readfds);
            this->clientfds.push_back(connfd);
            vector<string> temp;
            this->msg[connfd] = temp;
        }
        for (auto fd: this->clientfds) {
            if (FD_ISSET(fd, &readfds)) {
                try {
                    string data;
                    char dataChar[1024] = {};
                    // TODO:应该先处理read的data，再execute
                    Read(fd, dataChar, 1024);
                    data = dataChar;
                    if (!data.empty()) {
                        string temp = this->execute(data);
                        this->msg[fd].push_back(temp);
                        if (!FD_ISSET(fd, &this->writefds)) {
                            FD_SET(fd, &this->writefds);
                        }
                    }
                } catch (...) {
                    FD_CLR(fd, &this->readfds);
                    if (FD_ISSET(fd, &this->writefds)) {
                        FD_CLR(fd, &this->writefds);
                    }
                    Close(fd);
                    this->msg.erase(fd);
                    this->clientfds.erase(find(this->clientfds.begin(), this->clientfds.end(), fd));
                }
            }
        }
        for (auto fd: this->clientfds) {
            if (FD_ISSET(fd, &writefds)) {
                if (this->msg.find(fd) == this->msg.end()) {
                    continue;
                }
                int msgNum = 0;
                if (0 == (msgNum = this->msg[fd].size())) {
                    continue;
                }
                try {
                    for (int i = 0; i < msgNum; ++i) {
                        string msg = this->msg[fd].back();
                        char msgChar[1024] = {};
                        strcpy(msgChar, msg.c_str());
                        Writen(fd, msgChar, msg.length());
                        this->msg[fd].pop_back();
                    }
                } catch (...) {
                    if (FD_ISSET(fd, &this->writefds)) {
                        FD_CLR(fd, &this->writefds);
                    }
                }

            }
        }
    }
}


string RedisServer::execute(string data) {
    log("RedisServer execute(), " + data);
    vector<string> command;
    command = decode(data);

    int length = command.size();
    string method, key, value;
    if (0 == length) {
        return "-Error message\r\n";
    } else if (2 == length) {
        method = command.at(0);
        key = command.at(1);
    } else if (3 == length) {
        method = command.at(0);
        key = command.at(1);
        value = command.at(2);
    }

    if ("get" == method) {
        if (this->db.find(key) != this->db.end()) {
            return encode(this->db[key]);
        } else {
            return encode("None");
        }
    } else if ("del" == method) {
        if (this->db.find(key) != this->db.end()) {
            this->db.erase(this->db.find(key));
            return encode(1);
        } else {
            return encode(0);
        }
    } else if ("set" == method) {
        this->db[key] = value;
        return encode("OK");
    }

    return "-Error message\r\n";
}
