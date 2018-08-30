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


ssize_t Read(int fd, void *ptr, size_t nbytes) {
    log("wrap.cpp Read()");
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
    log("wrap.cpp Write()");
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
    log("wrap.cpp Readn()");
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
    log("wrap.cpp Writen()");
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
