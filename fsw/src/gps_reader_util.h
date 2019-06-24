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

int try_open(const char *portname);
int try_close(int fd);
int set_serialport_params(int fd);

double decimal_minutes2decimal_decimal(const double decimal_minutes);
size_t fill_serial_buffer(int fd, char *serialBuffer, size_t len);

#endif // _GPS_READER_UTIL_H_
