#ifndef _GPS_READER_PLATFORM_CONFIG_H_
#define _GPS_READER_PLATFORM_CONFIG_H_

#include <termios.h>

#ifdef __arm__ // raspberry pi
#define SERIAL_PORT_NAME "/dev/serial0"
#else // x86 with usb adaptor
#define SERIAL_PORT_NAME "/dev/ttyUSB0"
#endif

#define SPEED B9600
#define PARITY 0

#endif // _GPS_READER_PLATFORM_CONFIG_H_
