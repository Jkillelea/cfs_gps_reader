/*
 * Serial port routines for the GPS reader app
 */

#include "common_types.h"

#ifndef GPS_READER_SERIAL_H_
#define GPS_READER_SERIAL_H_

int32 GPS_READER_OpenPort(const char *const port);
int32 GPS_READER_ReadSerial(int32 fd, char *serialBuffer, int32 bufferLen);
int32 GPS_READER_ClosePort(int32 fd);

#endif // GPS_READER_SERIAL_H_

