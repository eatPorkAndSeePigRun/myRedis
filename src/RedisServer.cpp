#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <vector>
#include <string>
#include <map>
#include "RedisServer.h"
#include "wrap.h"
#include "code.h"

using namespace std;


RedisServer::RedisServer(uint32_t ip, uint32_t port) {
    this->ip = ip;
    this->port = port;
    this->is_open = true;
    this->is_close = true;
}


RedisServer::~RedisServer() {
    this->close();
}


void RedisServer::open() {
    if (!this->is_close) {
        return;
    }
    this->is_close = false;
    this->listenfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK);
    bzero(&(this->servaddr), sizeof(this->servaddr));
    this->servaddr.sin_family = AF_INET;
    this->servaddr.sin_addr.s_addr = htonl(this->ip);
    this->servaddr.sin_port = htons(this->port);
    Bind(this->listenfd, (struct sockaddr *) &(this->servaddr), sizeof(this->servaddr));
    Listen(this->listenfd, 20);
}


void RedisServer::close() {
    if (!this->is_open) {
        return;
    }
    this->is_open = false;
    // TODO
    for(auto fd: this->clientfds){
        Close(fd);
    }
    Close(this->listenfd);
}


void RedisServer::run() {
    while (true) {
        Select(this->listenfd, this->readfds, this->writefds, NULL, NULL);
        if (FD_ISSET(this->listenfd, this->readfds)) {
            struct sockaddr_in cliaddr;
            socklen_t cliaddr_len;
            int connfd = Accept4(this->listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len, SOCK_NONBLOCK);
            this->clientfds.push_back(connfd);
            vector<string> temp;
            this->msg[connfd] = temp;
        }
        for (auto fd: this->clientfds) {
            if (FD_ISSET(fd, this->readfds)) {
                string data;
                Readn(fd, (char *) data.data(), 1024);
                if (!data.empty()) {
                    string temp = this->execute(data);
                    this->msg[fd].push_back(temp);
                    if (FD_ISSET(fd, this->writefds)) {
                        FD_SET(fd, this->writefds);
                    }
                }
            }
            if (FD_ISSET(fd, this->writefds)) {
                if (this->msg.find(fd) != this->msg.end()) {
                    break;
                }
                string msg = this->msg[fd].back();
                Writen(fd, (const char *) msg.data(), 1024);
                this->msg[fd].pop_back();
                FD_CLR(fd, this->writefds);
            }
        }
    }
}


string RedisServer::execute(string data) {
    vector<string> command;
    command = decode(data);

    int length = command.size();
    string method, key, value;
    if (0 == length) {
        return "-Error message\r\n";
    } else if (2 == length) {
        method = command[0];
        key = command[1];
    } else if (3 == length) {
        method = command[0];
        key = command[1];
        value = command[2];
    }

    if (method == "get") {
        if (this->db.find(key) != this->db.end()) {
            return encode(this->db[key]);
        } else {
            return encode("None");
        }
    } else if (method == "del") {
        if (this->db.find(key) != this->db.end()) {
            this->db.erase(this->db.find(key));
            return encode(1);
        } else {
            return encode(0);
        }
    } else if (method == "set") {
        this->db[key] = value;
        return encode("OK");
    }

    return "-Error message\r\n";
}