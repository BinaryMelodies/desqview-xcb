
/* thin wrapper around the DESQview socket API */
/* most of it is documented in Ralf Brown's interrupt list */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <dos.h>
#if __GNUC__
# include <libi86/string.h>
#endif

#include "xcb_dos.h"

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

/* define DEBUG to display debug messages for the API calls */
//#define DEBUG 1

enum
{
    DVX_SOCKET_API = 0xDE2E,
    DVX_SOCKET_SELECT = 0x05,
    DVX_SOCKET_CLOSE = 0x06,
    DVX_SOCKET_RECVFROM = 0x09,
    DVX_SOCKET_SENDTO = 0x0D,
    DVX_SOCKET_CONNECT = 0x14,
    DVX_NET_EXIT = 0xFFFF,
};

/* convenience macros to make the purpose of the following fields clear */
// field used to pass values to DESQview
#define IN
// field used to return values from DESQview
#define OUT
// field used to pass and return values between DESQview
#define VAR
// 16-bit field that can only contain 0 or 1
#define BOOL  int16_t
#define FALSE ((BOOL)0)
#define TRUE  ((BOOL)1)

// 0x05
struct dvx_select_record
{
    OUT int16_t result_nfds;
    IN  int16_t nfds;
    IN  uint32_t readfds;
    IN  uint32_t writefds;
    IN  uint32_t exceptfds;
    IN  BOOL timeout_present;
    // these two fields are used to pass the timeout structure in the Quarterdeck implementation
    IN  uint32_t timeout_sec;
    IN  uint32_t timeout_usec;
} PACKED;

// 0x06
struct dvx_close_record
{
    OUT uint16_t status;
    IN  int16_t socket_handle;
};

// 0x09
struct dvx_recvfrom_record
{
    OUT int16_t bytes_read;
    IN  int16_t socket_handle;
    IN  uint16_t byte_count;
    IN  int16_t flags;
    IN  BOOL source_present;
    IN  uint16_t source_address_length;
    IN  char source_address[110];
    OUT char buffer[1024];
};

// 0x0D
struct dvx_sendto_record
{
    OUT int16_t bytes_written;
    IN  int16_t socket_handle;
    IN  uint16_t byte_count;
    IN  uint16_t remaining_bytes;
    IN  int16_t flags;
    IN  BOOL destination_present;
    IN  BOOL broadcast;
    IN  uint16_t destination_address_length;
    IN  char destination_address[110];
    IN  char buffer[1024];
};

// 0x14
struct dvx_connect_record
{
    OUT int16_t socket_handle;
    OUT int16_t screen_number;
    OUT int16_t display_number;
    // the following output fields are used this way in the Quarterdeck implementation
    OUT int16_t family;
    OUT int16_t saddrlen;
    IN  char display_name[256];
    OUT char fullname[256];
    OUT char saddr[256]; // unsure about size
};

struct dvx_socket_record
{
    char signature[2]; // 0xF0AD
    uint16_t function_number;
    uint16_t error_code;
    uint16_t maximum_message_size; // unsure
    uint16_t psp_segment;
    uint16_t jft_size;
    void __far * jft_address;
    void __far * mailbox_handle;
    void __far * timer_handle;
    union
    {
        struct dvx_select_record select;
        struct dvx_close_record close;
        struct dvx_recvfrom_record recvfrom;
        struct dvx_sendto_record sendto;
        struct dvx_connect_record connect;
    } u;
};

char __far * _fstrncpy(char __far * dst, const char __far * src, size_t sz)
{
    size_t srclen = _fstrlen(src);
    _fmemcpy(dst, src, MIN(srclen, sz));
    return dst;
}

/* C wrapper for the DESQview socket API: call with NULL for the first pointer to allocate a new record, the record_length_p may be NULL */
static inline struct dvx_socket_record __far * dvx_socket_call(struct dvx_socket_record __far * socket_record, uint16_t * record_length_p)
{
    union REGS r;

    r.x.ax = DVX_SOCKET_API;
    r.x.bx = FP_OFF(socket_record);
    r.x.dx = FP_SEG(socket_record);
    int86(0x15, &r, &r);

    if(record_length_p)
        *record_length_p = r.x.cx;

    return MK_FP(r.x.dx, r.x.bx);
}

/* static record that needs allocating a single time, handled by socket_connect and socket_close */
static uint16_t socket_record_length;
static struct dvx_socket_record __far * socket_record;

/* POSIX select call */
int socket_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout)
{
#if DEBUG
    fprintf(stderr, "socket_select(%d, %lX, %lX, %lX, %d)\n", nfds, readfds ? *readfds : 0, writefds ? *writefds : 0, exceptfds ? *exceptfds : 0, timeout ? timeout->tv_sec : -1);
#endif
    socket_record->error_code = errno;
    do
    {
        socket_record->function_number = DVX_SOCKET_SELECT;
        socket_record->u.select.nfds = nfds;
        socket_record->u.select.readfds = readfds ? *readfds : 0;
        socket_record->u.select.writefds = writefds ? *writefds : 0;
        socket_record->u.select.exceptfds = exceptfds ? *exceptfds : 0;
        if(timeout != NULL)
        {
            socket_record->u.select.timeout_present = TRUE;
            socket_record->u.select.timeout_sec = timeout->tv_sec;
            socket_record->u.select.timeout_usec = timeout->tv_usec;
        }
        else
        {
            socket_record->u.select.timeout_present = FALSE;
        }
        dvx_socket_call(socket_record, NULL);
        /* DESQview seems to not block when timeout == NULL even if none of the handles are ready, so we have to use a loop */
    } while(timeout == NULL && socket_record->u.select.result_nfds == 0);
    errno = socket_record->error_code;
    if(readfds) *readfds = socket_record->u.select.readfds;
    if(writefds) *writefds = socket_record->u.select.writefds;
    if(exceptfds) *exceptfds = socket_record->u.select.exceptfds;
    return socket_record->u.select.result_nfds;
}

/* POSIX close call for sockets, also terminates the socket API */
int socket_close(int socket)
{
    int status;
#if DEBUG
    fprintf(stderr, "socket_close(%d)\n", socket);
#endif
    socket_record->function_number = DVX_SOCKET_CLOSE;
    socket_record->u.close.socket_handle = socket;
    socket_record->error_code = errno;
    dvx_socket_call(socket_record, NULL);
    errno = socket_record->error_code;
    status = socket_record->u.close.status;

    /* terminates the statically allocated socket */
    socket_record->function_number = DVX_NET_EXIT;
    dvx_socket_call(socket_record, NULL);
    socket_record = NULL;

    return status;
}

/* POSIX recv call */
int socket_recv(int socket, void * buffer, size_t byte_count, int flags)
{
#if DEBUG
    fprintf(stderr, "socket_recv(%d, %X, %d, %d)\n", socket, buffer, byte_count, flags);
#endif
    socket_record->function_number = DVX_SOCKET_RECVFROM;
    socket_record->u.recvfrom.socket_handle = socket;
    socket_record->u.recvfrom.byte_count = byte_count;
    socket_record->u.recvfrom.flags = 0;
    socket_record->u.recvfrom.source_present = FALSE;
    socket_record->error_code = errno;
    dvx_socket_call(socket_record, NULL);
#if DEBUG
    fprintf(stderr, "? %d, errno: %d\n", socket_record->u.sendto.bytes_written, socket_record->error_code);
#endif
    errno = socket_record->error_code;
    _fmemcpy(buffer, socket_record->u.recvfrom.buffer, MIN(sizeof socket_record->u.recvfrom.buffer, byte_count));
    return socket_record->u.recvfrom.bytes_read;
}

/* POSIX send call, the remaining_bytes field reports if there are further bytes in the message that will be sent at a successive call */
int socket_send(int socket, const void * buffer, size_t byte_count, size_t remaining_bytes, int flags)
{
#if DEBUG
    fprintf(stderr, "socket_send(%d, %X, %d, %d, %d)\n", socket, buffer, byte_count, remaining_bytes, flags);
#endif
    socket_record->function_number = DVX_SOCKET_SENDTO;
    socket_record->u.sendto.socket_handle = socket;
    socket_record->u.sendto.byte_count = byte_count;
    socket_record->u.sendto.remaining_bytes = 0;
    socket_record->u.sendto.flags = 0;
    socket_record->u.sendto.destination_present = FALSE;
    socket_record->u.sendto.broadcast = FALSE;
    _fmemcpy(socket_record->u.sendto.buffer, buffer, MIN(sizeof socket_record->u.sendto.buffer, byte_count));
    socket_record->error_code = errno;
    dvx_socket_call(socket_record, NULL);
    errno = socket_record->error_code;
    return socket_record->u.sendto.bytes_written;
}

/* POSIX socket/connect call, corresponds to the Xlib internal _XConnectDisplay call, this function also initializes the socket API */
int socket_connect(const char * display_name, int * screenp)
{
    if(socket_record == 0)
    {
        /* initializes a static record to be used for accessing the socket API */

        socket_record = dvx_socket_call(NULL, &socket_record_length);

        if(socket_record == 0)
        {
            fprintf(stderr, "Unable to start the DESQview network API\n");
            exit(1);
        }
    }

    socket_record->function_number = DVX_SOCKET_CONNECT;
    _fstrncpy(socket_record->u.connect.display_name, display_name, sizeof socket_record->u.connect.display_name);
    socket_record->error_code = errno;
    dvx_socket_call(socket_record, NULL);
    errno = socket_record->error_code;
    if(screenp)
        *screenp = socket_record->u.connect.screen_number;
    return socket_record->u.connect.socket_handle;
}

ssize_t readv(int fd, const struct iovec * io, int iovcnt)
{
    ssize_t count = 0;
    int i;
    for(i = 0; i < iovcnt; i++)
    {
        ssize_t single_count;
        if(io[i].iov_len == 0)
            continue;
        single_count = socket_recv(fd, io[i].iov_base, io[i].iov_len, 0);
        if(single_count < 0)
            return single_count;
        count += single_count;
        if(single_count < io[i].iov_len)
            break;
    }
    return count;
}

ssize_t writev(int fd, const struct iovec * io, int iovcnt)
{
    uint16_t total_size = 0;
    ssize_t count = 0;
    int i;
    for(i = 0; i < iovcnt; i++)
    {
        total_size += io[i].iov_len;
    }
    for(i = 0; i < iovcnt; i++)
    {
        ssize_t single_count;
        if(io[i].iov_len == 0)
            continue;
        single_count = socket_send(fd, io[i].iov_base, io[i].iov_len, total_size, 0);
        if(single_count < 0)
            return single_count;
        count += single_count;
        if(single_count < io[i].iov_len)
            break;
        total_size -= io[i].iov_len;
    }
    return count;
}

