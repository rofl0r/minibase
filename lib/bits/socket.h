#ifndef __BITS_SOCKET_H__
#define __BITS_SOCKET_H__

#include <bits/types.h>
#include <bits/socket/unspec.h>

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_SEQPACKET  5
#define SOCK_PACKET    10

#define SOCK_NONBLOCK  (1<<11)
#define SOCK_CLOEXEC   (1<<19)

#define SOL_SOCKET      1
#define SO_REUSEADDR    2
#define SO_BROADCAST    6
#define SO_PASSCRED     16
#define SO_PEERCRED     17
#define SO_BINDTODEVICE 25

#define MSG_OOB            (1<<0)
#define MSG_DONTWAIT       (1<<6)
#define MSG_NOSIGNAL       (1<<14)
#define MSG_CMSG_CLOEXEC   (1<<30)

#endif
