#ifndef _GPS_READER_CONSTANTS_H_
#define _GPS_READER_CONSTANTS_H_

#include <termios.h>

extern const char *portname;
extern const int SPEED;
extern const int PARITY;

// vscode doesn't find this one flag in bits/termios.h for some reason.
#ifndef CRTSCTS
#warning "this bit of code is for keeping VSCode happy and shouldn't compile!"
#define CRTSCTS  020000000000
#endif

#endif // _GPS_READER_CONSTANTS_H_
