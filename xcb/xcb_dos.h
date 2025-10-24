#ifndef __XCB_DOS_H__
#define __XCB_DOS_H__

/* use a 32-bit fd_set implementation as the one for DESQview/X instead of the one provided by the GNU libraries */
#if __GNUC__
#ifdef fd_set
#undef fd_set
#endif
#define fd_set __gnu_fd_set
#endif
#include <sys/types.h>
#if __GNUC__
#undef fd_set
#endif

typedef uint32_t fd_set;
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 32
#ifdef FD_CLR
#undef FD_CLR
#endif
#define FD_CLR(fd, set) (*(set) &= ~(1 << (fd)))
#ifdef FD_ISSET
#undef FD_ISSET
#endif
#define FD_ISSET(fd, set) ((*(set) >> (fd)) & 1)
#ifdef FD_SET
#undef FD_SET
#endif
#define FD_SET(fd, set) (*(set) |= (1 << (fd)))
#ifdef FD_ZERO
#undef FD_ZERO
#endif
#define FD_ZERO(set) (*(set) = 0)

/* adjust some of the error codes returned by DESQview/X */
#undef EDEADLK
#define EDEADLK 35

#if __WATCOMC__
#define ECONNREFUSED 111
#endif

/* DESQview/X API calls */

#if __WATCOMC__
struct timeval
{
    uint32_t tv_sec;
    uint32_t tv_usec;
};
#endif

int socket_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout);
int socket_close(int socket);
int socket_recv(int socket, void * buffer, size_t byte_count, int flags);
int socket_send(int socket, const void * buffer, size_t byte_count, size_t remaining_bytes, int flags);
int socket_connect(const char * display_name, int * screenp);

/* custom implementation of readv/writev */

struct iovec
{
    void * iov_base;
    size_t iov_len;
};

ssize_t readv(int fd, const struct iovec * io, int iovcnt);
ssize_t writev(int fd, const struct iovec * io, int iovcnt);

/* removing pthread references, as they are not needed for DESQview/X */

#define pthread_mutex_lock(...) ((void) 0)
#define pthread_mutex_unlock(...) ((void) 0)
#define pthread_mutex_init(...) (0)
#define pthread_mutex_destroy(...) ((void) 0)
#define pthread_cond_broadcast(...) ((void) 0)
#define pthread_cond_wait(...) ((void) 0)
#define pthread_cond_init(...) (0)
#define pthread_cond_destroy(...) ((void) 0)
#define pthread_cond_signal(...) ((void) 0)

#endif /* __XCB_DOS_H__ */
