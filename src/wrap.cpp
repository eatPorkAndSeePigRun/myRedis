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
    ssize_t n;
    while ((n = read(fd, ptr, nbytes)) == -1) {
        if (EINTR == errno || EAGAIN == errno) {
            continue;
        } else {
            log("***error***   Read error: " + string(strerror(errno)));
            return -1;
        }
    }
    log("wrap.cpp Read(), fd: " + to_string(fd) + " nbytes: " + to_string(nbytes) + " ptr: " + string((char *) ptr));
    return n;
}


ssize_t Write(int fd, const void *ptr, size_t nbytes) {
//    log("wrap.cpp Write(), fd: " + to_string(fd) + " ptr: " + string((char *) ptr) + " nbytes: " + to_string(nbytes));
    ssize_t n;
    while ((n = write(fd, ptr, nbytes)) == -1) {
        if (EINTR == errno || EAGAIN == errno) {
            continue;
        } else {
            log("***error***   Read error: " + string(strerror(errno)));
            return -1;
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
    log("wrap.cpp Readn(), fd: " + to_string(fd) + " vptr: " + string((char *) vptr) + " nbytes: " + to_string(nbytes));
    return (nbytes - nleft);
}

ssize_t Writen(int fd, const char *vptr, size_t nbytes) {
    log("wrap.cpp Writen(), fd: " + to_string(fd)+ " nbytes: " + to_string(nbytes) + " vptr: " + string((char *) vptr));
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
