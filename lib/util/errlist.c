#include <errtag.h>
#include <util.h>

/* Default error list for those utils that do not need
   a custom one. This should include common error for
   common syscalls. */

const char errlist[] __attribute__((weak)) = {
	NE2BIG
	NEACCES
	NEADDRINUSE
	NEAGAIN
	NEALREADY
	NEBADF
	NEBADMSG
	NEBUSY
	NECONNREFUSED
	NEEXIST
	NEFAULT
	NEFBIG
	NEINTR
	NEINVAL
	NEIO
	NEISDIR
	NELOOP
	NEMFILE
	NENAMETOOLONG
	NENETDOWN
	NENFILE
	NENOBUFS
	NENODEV
	NENOENT
	NENOEXEC
	NENOKEY
	NENOMEM
	NENOSPC
	NENOSYS
	NENOTCONN
	NENOTDIR
	NENOTTY
	NENXIO
	NEOPNOTSUPP
	NEPERM
	NEPIPE
	NERANGE
	NEROFS
	NESRCH
	NETIMEDOUT
	NETXTBSY
	"\0"
};
