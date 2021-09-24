#include "cfe_evs.h"
#include "trans_rs422.h"
#include "gps_reader_platform_cfg.h"

int32 GPS_READER_OpenPort(const char *const port)
{
    IO_TransRS422Config_t rs422Config =
    {
        .device   = SERIAL_PORT_NAME,
        .baudRate = 9600,
        .cFlags   = CS8 | CREAD,
        .timeout  = 1000,
        .minBytes = 0,
    };

    return IO_TransRS422Init(&rs422Config);
}

int32 GPS_READER_ReadSerial(int32 fd, char *serialBuffer, int32 bufferLen)
{
    int32 bytesRead = 0;

    bytesRead = IO_TransRS422Read(fd, (uint8 *) serialBuffer, bufferLen);

    return bytesRead;
}

int32 GPS_READER_ClosePort(int32 fd)
{
    return IO_TransRS422Close(fd);
}
