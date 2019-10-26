#ifndef _ti_socket_h
#define _ti_socket_h 1

// data received lands in this buffer.
// it is also overwritten to form outgoing messages.
extern unsigned char tcpbuf[];

unsigned int tcp_connect(unsigned int socketId, unsigned char* hostname, unsigned char* port);

unsigned int tcp_close(unsigned int socketId);

// will send at most 122 byte character sequences (cause size of output buffer)
int tcp_send_chars(unsigned int socketId, unsigned char* buf, int size);

int tcp_read_socket(unsigned int socketId);

#endif
