#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <asm/errno.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "wrap.h"
#include "log.h"
using namespace std;


void log_then_exit(string msg){
    string errcontent = strerror(errno);
    msg = msg.substr(0, msg.length()-1);
    log(msg+errcontent);
    exit(1);
}


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;
    if((n = accept(fd, sa, salenptr)) < 0){
        log_then_exit("Accept error:");
    }
    return n;
}


int Accept4(int fd, struct sockaddr *sa, socklen_t *salenptr, int flags){
    int n;
    if((n = accept4(fd, sa, salenptr, flags)) < 0){
        log_then_exit("Accept4 error:");
    }
    return n;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (bind(fd, sa, salen) < 0){
        log_then_exit("Bind error:");
    }
}


void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (connect(fd, sa, salen) < 0){
        log_then_exit("Connect error:");
    }
}


void Listen(int fd, int backlog) {
    if (listen(fd, backlog) < 0){
        log_then_exit("Listen error:");
    }
}


int Socket(int family, int type, int protocol) {
    int n;
    if ((n = socket(family, type, protocol)) < 0){
        log_then_exit("Socket error:");
    }
    return n;
}


void Close(int fd) {
    if (close(fd) < 0){
        log_then_exit("close error:");
    }
}


ssize_t Read(int fd, void *ptr, size_t nbytes) {
    ssize_t n;
    while((n = read(fd, ptr, nbytes)) < 0 ){
        if(EINTR == errno || EAGAIN == errno){
            continue;
        }else{
            string temp = strerror(errno);
            log("Read error:"+temp);
            return n;
        }
    }
    return n;
}


ssize_t Write(int fd, const void *ptr, size_t nbytes) {
    ssize_t n;
    while((n = write(fd, ptr, nbytes)) < 0 ){
        if(EINTR == errno || EAGAIN == errno){
            continue;
        }else{
            string temp = strerror(errno);
            log("Read error:"+temp);
            return n;
        }
    }
    return n;
}


ssize_t Readn(int fd, char *vptr, size_t nbytes) {
    size_t nleft;
    size_t nread;
    char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nread = Read(fd, ptr, nleft)) < 0) {
            if (EINTR == errno || EAGAIN == errno){
                nread = 0;
            }else{
                return -1;
            }
        } else if (nread == 0){
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (nbytes - nleft);
}

ssize_t Writen(int fd, const char *vptr, size_t nbytes) {
    size_t nleft;
    size_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nwritten = Write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && (EINTR == errno || EAGAIN == errno)){
                nwritten = 0;
            }else{
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return nbytes;
}



int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
    int n;
    if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0){
        log_then_exit("Select error:");
    }
    return n;
}

