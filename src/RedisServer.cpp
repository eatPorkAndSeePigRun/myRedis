#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "RedisServer.h"
#include "wrap.h"
#include "code.h"


using namespace std;


RedisServer::RedisServer(uint32_t ip, uint32_t port){
    this->ip = ip;
    this->port = port;
    this->is_open = true;
    this->is_close = true;
}


RedisServer::~RedisServer(){
}


void RedisServer::open(){
    if(!this->is_close){
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


void RedisServer::close(){
    if(!this->is_open){
        return;
    }
    this->is_open = false;
    // TODO
}


void RedisServer::run(){
    int nready;
    while(true){
        nready = Select(this->listenfd, this->readfds, this->writefds, NULL, NULL);
        for(auto fd: *this->readfds){
            if(fd == this->listenfd){
                struct sockaddr_in cliaddr;
                socklen_t cliaddr_len;
                int connfd = Accept4(this->listenfd, (struct sockaddr *) cliaddr, &cliaddr_len, SOCK_NONBLOCK);
                FD_SET(connfd, &this->readfds);
                vector<string> temp;
                this->msg[conn] = temp;    
            }else{
                string data;
                Readn(fd, data, data.length());
                if(data != NULL){
                    string temp = this->execute(data);
                    this->msg[fd].push_back(temp);
                    if(FD_ISSET(fd, &this->writefds)){
                        FD_SET(fd, &this->writefds);
                    }
                }
            }
        }
        for(auto fd: *this->writefds){
            if(this->msg.count() != 1){
                break;
            }       
            string msg = this->msg[fd].pop_back();
            Writen(fd, msg, msg.length());
        }
    }
}


string RedisServer::execute(vector<string> command){
    int length = command.sizeof();
    string method, key, value;
    if(0 == length){
        return "-Error message\r\n"
    }else if(2 == length){
        method = command.pop_back();
        key = command.pop_back();
    }else if(3 == length){
        method = command.pop_back();
        key = command.pop_back();
        value = command.pop_back();
    } 
    if(method == "get"){
        if(this->db.find(key) != this->db.end()){                 
            return encode(this->db[key]);
        }else{
            return encode("None");
        }
    }else if(method == "del"){
        if(this->db.find(key) != this->db.end()){
            this.db.erase(this->db.find(key));                    
            return encode(1);
        }else{
            return encode(0);
        }
    }else if(method == "set"){
        this->db[key] = value;
        return encode("OK");
    }  
}
