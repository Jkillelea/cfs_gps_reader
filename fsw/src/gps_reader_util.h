#ifndef _GPS_READER_UTIL_H_
#define _GPS_READER_UTIL_H_

#include "cfe.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ssize_t find_string_start(const char *buf, const char *substr, size_t buf_size, size_t substr_size);
// return index of last '\r\n' (returns idx of the '\r')
ssize_t find_last_crlf(const char *buf, size_t buf_size);

int try_open(const char *portname);
int try_close(int fd);

double decimal_minutes2decimal_decimal(const double decimal_minutes);
size_t fill_serial_buffer(int fd, const char *serialBuffer, size_t len);

#endif // _GPS_READER_UTIL_H_
