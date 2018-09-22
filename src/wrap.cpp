#include <system_error>
#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <cerrno>
#include <string>
#include "wrap.h"
#include "log.h"

using namespace std;


ssize_t Read(int fd, void *ptr, size_t nbytes) {
    //signal(SIGPIPE, SIG_IGN);
    errno = 0;
    ssize_t n = 0;
    while (true) {
        n = read(fd, ptr, nbytes);
        if (n < 0) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            } else {
                break;
            }
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
    //signal(SIGPIPE, SIG_IGN);
    errno = 0;
    ssize_t n;
    n = write(fd, ptr, nbytes);
    if (n < 0) {
        if (EINTR == errno || EAGAIN == errno) {
            n = 0;
        }
    }
    stringstream ss;
    ss << " fd: " << fd << " n: " << n << " ptr: " << &ptr << " errno: " << errno;
    log("wrap.cpp Write()," + ss.str());
    return n;
}

/*
ssize_t Readn(int fd, char *vptr, size_t nbytes) {
    errno = 0;
    size_t nleft;
    size_t nread;
    char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        if ((nread = Read(fd, ptr, nleft)) < 0) {
            if (EINTR == errno) {
                errno = 0;
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
*/

/*
ssize_t Writen(int fd, const char *vptr, size_t nbytes) {
    errno = 0;
	//stringstream ss;
	//ss << " fd: " << fd << " nbytes: " << nbytes << " vptr: " << &vptr << " errno: " << errno;
    //log("wrap.cpp Writen()," + ss.str());
    size_t nleft;
    size_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = nbytes;
    while (nleft > 0) {
        nwritten = Write(fd, ptr, nleft);
        //ss.str("");
        //ss << " nwritten: " << nwritten << " errno: " << errno;
        //log("wrap.cpp Writen(), nwritten:" + ss.str());
        if (nwritten <= 0 || nwritten == 18446744073709551615) {
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return nbytes;
}
*/
