#ifndef _GPS_READER_MSGS_H_
#define _GPS_READER_MSGS_H_

#include "cfe.h"
#include "cfe_msg.h"
#include "cfe_msg_api_typedefs.h"
#include "nmea/nmea.h"

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaINFO gpsInfo;
} GpsInfoMsg_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaGPGGA gpsGpgga;
} GpsGpggaMsg_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaGPGSA gpsGpgsa;
} GpsGpgsaMsg_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaGPGSV gpsGpgsv;
} GpsGpgsvMsg_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaGPRMC gpsGprmc;
} GpsGprmcMsg_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlmHeader;
    nmeaGPVTG gpsGpvtg;
} GpsGpvtgMsg_t;


#endif // _GPS_READER_MSGS_H_
