#ifndef WRAP_H
#define WRAP_H


ssize_t Read(int fd, void *ptr, size_t nbytes);

ssize_t Write(int fd, const void *ptr, size_t nbytes);

ssize_t Readn(int fd, char *vptr, size_t nbytes);

ssize_t Writen(int fd, const char *vptr, size_t n);


#endif //WRAP_H
