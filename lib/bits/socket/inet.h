#include <bits/types.h>

#define PF_INET 2
#define AF_INET 2

#define IPPROTO_UDP 17

struct sockaddr_in {
	uint16_t family;
	uint16_t port;  /* BIG-ENDIAN! */
	uint8_t addr[4];
	char _[8];      /* to match sizeof(struct sockaddr) */
};
