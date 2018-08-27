#include <sys/socket.h>
#include <asm/errno.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <string>
#include "wrap.h"
#include "log.h"

using namespace std;


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    log("wrap Accept()");
    int n;
    if ((n = accept(fd, sa, salenptr)) < 0) {
        log("***error***   Accept error: " + string(strerror(errno)));
    }
    return n;
}


int Accept4(int fd, struct sockaddr *sa, socklen_t *salenptr, int flags) {
    log("wrap Accept4()");
    int n;
    if ((n = accept4(fd, sa, salenptr, flags)) < 0) {
        log("***error***   Accept4 error: " + string(strerror(errno)));
    }
    return n;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
    log("wrap Bind()");
    if (bind(fd, sa, salen) < 0) {
        log("***error***   Bind error: " + string(strerror(errno)));
        exit(1);
    }
}


void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    log("wrap Connect()");
    if (connect(fd, sa, salen) < 0) {
        log("***error***   Connect error: " + string(strerror(errno)));
        exit(1);
    }
}


void Listen(int fd, int backlog) {
    log("wrap Listen()");
    if (listen(fd, backlog) < 0) {
        log("***error***   Listen error: " + string(strerror(errno)));
        exit(1);
    }
}


int Socket(int family, int type, int protocol) {
    log("wrap Socket()");
    int n;
    if ((n = socket(family, type, protocol)) < 0) {
        log("***error***   Socket error: " + string(strerror(errno)));
        exit(1);
    }
    return n;
}


void Close(int fd) {
    log("wrap Close()");
    if (close(fd) < 0) {
        log("***error***   close error: " + string(strerror(errno)));
        exit(1);
    }
}


ssize_t Read(int fd, void *ptr, size_t nbytes) {
    log("wrap Read()");
    ssize_t n;
    while ((n = read(fd, ptr, nbytes)) < 0) {
        if (EINTR == errno || EAGAIN == errno) {
            continue;
        } else {
            log("***error***   Read error: " + string(strerror(errno)));
            return n;
        }
    }
    return n;
}


ssize_t Write(int fd, const void *ptr, size_t nbytes) {
    log("wrap Write()");
    ssize_t n;
    while ((n = write(fd, ptr, nbytes)) < 0) {
        if (EINTR == errno || EAGAIN == errno) {
            continue;
        } else {
            log("***error***   Read error: " + string(strerror(errno)));
            return n;
        }
    }
    return n;
}


ssize_t Readn(int fd, char *vptr, size_t nbytes) {
    log("wrap Readn()");
    size_t nleft;
    size_t nread;
    char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nread = Read(fd, ptr, nleft)) < 0) {
            if (EINTR == errno || EAGAIN == errno) {
                nread = 0;
            } else {
                return -1;
            }
        } else if (0 == nread) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (nbytes - nleft);
}

ssize_t Writen(int fd, const char *vptr, size_t nbytes) {
    log("wrap Writen()");
    size_t nleft;
    size_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nwritten = Write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && (EINTR == errno || EAGAIN == errno)) {
                nwritten = 0;
            } else {
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return nbytes;
}


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    log("wrap Select()");
    int n;
    if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        log("***error***   Select error: " + string(strerror(errno)));
        exit(1);
    }
    return n;
}
