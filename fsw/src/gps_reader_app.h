#ifndef _GPS_READER_H_
#define _GPS_READER_H_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "gps_reader_events.h"
#include "gps_reader_version.h"
#include "gps_reader_perfids.h"
#include "gps_reader_msgs.h"
#include "gps_reader_msgids.h"
#include "gps_reader_platform_cfg.h"
#include "gps_reader_util.h"
#include "nmea/nmea.h"

struct {

    int serialFd;

    /* NMEA message parser */
    nmeaPARSER gpsParser;
    /* NMEA message buffer */
    char serialBuffer[GPS_READER_SERIAL_BUFFER_SIZE];

    /* Software Bus Objects */
    CFE_SB_PipeId_t rcvPipe;
    CFE_SB_Buffer_t *rcvMsgPtr;

    /* Software Bus Messages */
    GpsInfoMsg_t  gpsInfoMsg;
    GpsGpggaMsg_t gpsGpggaMsg;
    GpsGpgsaMsg_t gpsGpgsaMsg;
    GpsGpgsvMsg_t gpsGpgsvMsg;
    GpsGprmcMsg_t gpsGprmcMsg;
    GpsGpvtgMsg_t gpsGpvtgMsg;

} g_GPS_READER_Data;

void GPS_READER_AppMain(void);
CFE_Status_t GPS_READER_Init(void);
CFE_Status_t GPS_READER_RcvMsg(void);

#endif // _GPS_READER_H_
