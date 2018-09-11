#include <sys/socket.h>
#include <asm/errno.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include <cerrno>
#include <string>
#include "wrap.h"
#include "log.h"

using namespace std;


ssize_t Read(int fd, void *ptr, size_t nbytes) {
    signal(SIGPIPE, SIG_IGN);
    ssize_t n;
    while (true) {
        n = read(fd, ptr, nbytes);
        if (EINTR == errno || EAGAIN == errno) {
            errno = 0;
            continue;
        } else {
            break;
        }
    }
	stringstream ss;
	ss << " fd: " << fd << " n: " << n << " ptr: " << &ptr << " errno: " << errno;
    log("wrap.cpp Read()," + ss.str());
    return n;
}


ssize_t Write(int fd, const void *ptr, size_t nbytes) {
    signal(SIGPIPE, SIG_IGN);
    ssize_t n = write(fd, ptr, nbytes);
	stringstream ss;
	ss << " fd: " << fd << " n: " << n << " ptr: " << &ptr << " errno: " << errno;
    log("wrap.cpp Write()," + ss.str());
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
            if (EINTR == errno) {
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
	stringstream ss;
	ss << " fd: " << fd << " nbytes: " << nbytes << " vptr: " << &vptr;
    log("wrap.cpp Readn()," + ss.str());
    return (nbytes - nleft);
}

ssize_t Writen(int fd, const char *vptr, size_t nbytes) {
	stringstream ss;
	ss << " fd: " << fd << " nbytes: " << nbytes << " vptr: " << &vptr;
    log("wrap.cpp Writen()," + ss.str());
    size_t nleft;
    size_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nwritten = Write(fd, ptr, nleft)) <= 0) {
            if ((nwritten < 0) && (EINTR == errno)) {
                errno = 0;
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
