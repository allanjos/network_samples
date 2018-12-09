#ifndef NETWORK_H
#define NETWORK_H

#include <errno.h>

const char *get_socket_error_message(int error_code) {
    switch (error_code) {
        case EWOULDBLOCK:
            return "The socket is marked nonblocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received. POSIX.1-2001 allows either error to be returned for this case, and does not require these constants to have the same value, so a portable application should check for both possibilities.";
        case EBADF:
            return "The argument sockfd is an invalid descriptor.";
        case ECONNREFUSED:
            return "A remote host refused to allow the network connection (typically because it is not running the requested service).";
        case EFAULT:
            return "The receive buffer pointer(s) point outside the process's address space.";
        case EINTR:
            return "The receive was interrupted by delivery of a signal before any data were available; see signal(7).";
        case EINVAL:
            return "Invalid argument passed.";
        case ENOMEM:
            return "Could not allocate memory for recvmsg().";
        case ENOTCONN:
            return "The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).";
        case ENOTSOCK:
            return "The argument sockfd does not refer to a socket.";
    }

    return "";
}

#endif /* NETWORK_H */
