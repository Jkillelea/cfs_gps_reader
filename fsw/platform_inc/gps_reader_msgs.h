#ifndef _GPS_READER_MSGS_H_
#define _GPS_READER_MSGS_H_

#include "cfe.h"
#include "nmea/nmea.h"

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaINFO gpsInfo;
} GpsInfoMsg_t;

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaGPGGA gpsGpgga;
} GpsGpggaMsg_t;

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaGPGSA gpsGpgsa;
} GpsGpgsaMsg_t;

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaGPGSV gpsGpgsv;
} GpsGpgsvMsg_t;

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaGPRMC gpsGprmc;
} GpsGprmcMsg_t;

typedef struct {
    uint8 tlmHeader[CFE_SB_TLM_HDR_SIZE];
    nmeaGPVTG gpsGpvtg;
} GpsGpvtgMsg_t;


#endif // _GPS_READER_MSGS_H_
