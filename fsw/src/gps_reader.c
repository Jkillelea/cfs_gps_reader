#include "gps_reader.h"
#include "gps_reader_constants.h"
#include "gps_reader_util.h"
#include "gps_print_msgs.h"
#include "gps_reader_events.h"
#include "gps_reader_version.h"
#include "gps_reader_perfids.h"
#include "gps_reader_msgs.h"
#include "gps_reader_msgids.h"
#include "nmea/nmea.h"

nmeaPARSER gpsParser;

CFE_SB_PipeId_t commandPipe;
CFE_SB_MsgPtr_t msgPtr;

GpsInfoMsg_t  gpsInfoMsg;
GpsGpggaMsg_t gpsGpggaMsg;
GpsGpgsaMsg_t gpsGpgsaMsg;
GpsGprmcMsg_t gpsGprmcMsg;
GpsGpvtgMsg_t gpsGpvtgMsg;

char serialBuffer[GPS_READER_SERIAL_BUFFER_SIZE];

void GPS_READER_Main(void) {
    uint32 runStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

    GPS_READER_Init();

    int32 fd = try_open(portname);
    if (fd < 0) {
        CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_ERROR,
                        "Failed to open serial port %s", portname);
    }

    while (CFE_ES_RunLoop(&runStatus) == TRUE) {
        memset(serialBuffer, 0, GPS_READER_SERIAL_BUFFER_SIZE);
        nmea_zero_INFO(&gpsInfoMsg.gpsInfo);
        nmea_zero_GPGGA(&gpsGpggaMsg.gpsGpgga);
        nmea_zero_GPGSA(&gpsGpgsaMsg.gpsGpgsa);
        nmea_zero_GPRMC(&gpsGprmcMsg.gpsGprmc);
        nmea_zero_GPVTG(&gpsGpvtgMsg.gpsGpvtg);

        CFE_ES_PerfLogExit(GPS_READER_PERF_ID);

        int32 status = CFE_SB_RcvMsg(&msgPtr, commandPipe, 1000);

        CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

        if (status == CFE_SUCCESS) {
            CFE_SB_MsgId_t msgId = CFE_SB_GetMsgId(msgPtr);
            OS_printf("GPS_READER: Got a message, id 0x%X\n", msgId);
        }

        // read from serial port
        int32 nbytes = fill_serial_buffer(fd, serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE);

        // failed to read anything
        if (nbytes < 0) {
            CFE_EVS_SendEvent(GPS_READER_ERR_LOGMSG, CFE_EVS_ERROR,
                "No bytes read! Trying to repoen serial port %s...",
                portname);
            fd = try_open(portname);
            continue;
        } else {
            CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "Read %d bytes", nbytes);
            int32 nmessages = nmea_parse(&gpsParser,
                                         serialBuffer,
                                         GPS_READER_SERIAL_BUFFER_SIZE,
                                         &gpsInfoMsg.gpsInfo);
            if (nmessages > 0) {

                // OS_printf("%s\n", serialBuffer);

                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION,
                    "%d gps messages", nmessages);


                // send the Info message
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have info");
                CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsInfoMsg);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsInfoMsg);

                // check which messages were recieved
                if (gpsInfoMsg.gpsInfo.smask & GPGGA) {
                    CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have gpgga");
                    nmea_info2GPGGA(&gpsInfoMsg.gpsInfo, &gpsGpggaMsg.gpsGpgga);
                    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpggaMsg);
                    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpggaMsg);
                    // print_gpgga(&gpsGpggaMsg.gpsGpgga);
                }

                if (gpsInfoMsg.gpsInfo.smask & GPGSA) {
                    CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have gpgsa");
                    nmea_info2GPGSA(&gpsInfoMsg.gpsInfo, &gpsGpgsaMsg.gpsGpgsa);
                    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpgsaMsg);
                    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpgsaMsg);
                    // print_gpgsa(&gpsGpgsaMsg.gpsGpgsa);
                }

                // currently breaking without a link to libmath
                // if (gpsInfoMsg.gpsInfo.smask & GPGSV) {
                //     nmeaGPGSV gpgsv;
                //     CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have gpgsv");
                //     nmea_info2GPGSV(&gpsInfo, &gpgsv, 0);
                //     print_gpgsv(&gpgsv);
                // }

                if (gpsInfoMsg.gpsInfo.smask & GPRMC) {
                    CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have gprmc");
                    nmea_info2GPRMC(&gpsInfoMsg.gpsInfo, &gpsGprmcMsg.gpsGprmc);
                    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGprmcMsg);
                    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGprmcMsg);
                    // print_gprmc(&gpsGprmcMsg.gpsGprmc);
                }

                if (gpsInfoMsg.gpsInfo.smask & GPVTG) {
                    CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_INFORMATION, "have gpvtg");
                    nmea_info2GPVTG(&gpsInfoMsg.gpsInfo, &gpsGpvtgMsg.gpsGpvtg);
                    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpvtgMsg);
                    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpvtgMsg);
                    // print_gpvtg(&gpvtg);
                }
            }
        }
    }
}

void GPS_READER_Init(void) {
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    CFE_EVS_SendEvent(GPS_READER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "Startup. Version %d.%d.%d.%d",
                        GPS_READER_MAJOR_VERSION, GPS_READER_MINOR_VERSION,
                        GPS_READER_REVISION,      GPS_READER_MISSION_REV);

    CFE_SB_CreatePipe(&commandPipe, 10, "GPS_READER_PIPE");
    CFE_SB_Subscribe(SC_1HZ_WAKEUP_MID, commandPipe);

    nmea_parser_init(&gpsParser);

    CFE_SB_InitMsg(&gpsInfoMsg,  GPS_READER_GPS_INFO_MSG,  sizeof(gpsInfoMsg),  TRUE);
    CFE_SB_InitMsg(&gpsGpggaMsg, GPS_READER_GPS_GPGGA_MSG, sizeof(gpsGpggaMsg), TRUE);
    CFE_SB_InitMsg(&gpsGpgsaMsg, GPS_READER_GPS_GPGSA_MSG, sizeof(gpsGpgsaMsg), TRUE);
    CFE_SB_InitMsg(&gpsGprmcMsg, GPS_READER_GPS_GPRMC_MSG, sizeof(gpsGprmcMsg), TRUE);
    CFE_SB_InitMsg(&gpsGpvtgMsg, GPS_READER_GPS_GPVTG_MSG, sizeof(gpsGpvtgMsg), TRUE);
}
