#ifndef WRAP_H
#define WRAP_H

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string>
using namespace std;


void log_then_exit(string msg);


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

int Accept4(int fd, struct sockaddr *sa, socklen_t *salenptr, int flags);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);

int Socket(int family, int type, int protocol=0);

void Close(int fd);


ssize_t Read(int fd, void *ptr, size_t nbytes);

ssize_t Write(int fd, const char *ptr, size_t nbytes);


ssize_t Readn(int fd, void *vptr, size_t n);

ssize_t Writen(int fd, const char *vptr, size_t n);


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#endif //WRAP_H
