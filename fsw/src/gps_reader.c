#include "gps_reader.h"
#include "cfe_evs_extern_typedefs.h"
#include "cfe_sb.h"
#include "cfe_time.h"

/* NMEA message buffer */
char serialBuffer[GPS_READER_SERIAL_BUFFER_SIZE];

/* NMEA message parser */
nmeaPARSER gpsParser;

/* Software Bus Objects */
CFE_SB_PipeId_t rcvPipe;
CFE_SB_Buffer_t *rcvMsgPtr;

/* Software Bus Messages */
static GpsInfoMsg_t  gpsInfoMsg;
static GpsGpggaMsg_t gpsGpggaMsg;
static GpsGpgsaMsg_t gpsGpgsaMsg;
static GpsGpgsvMsg_t gpsGpgsvMsg;
static GpsGprmcMsg_t gpsGprmcMsg;
static GpsGpvtgMsg_t gpsGpvtgMsg;


void GPS_READER_Main(void)
{
    uint32 runStatus = CFE_ES_RunStatus_APP_RUN;

    CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

    GPS_READER_Init();

    /* Try and open the serial port */
    int32 fd = try_open(SERIAL_PORT_NAME);
    if (fd < 0)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                        "Failed to open serial port %s", SERIAL_PORT_NAME);
    }

    while (CFE_ES_RunLoop(&runStatus) == true)
    {
        /* Init objects for this loop */
        memset(serialBuffer, 0, GPS_READER_SERIAL_BUFFER_SIZE);
        nmea_zero_INFO(&gpsInfoMsg.gpsInfo);
        nmea_zero_GPGGA(&gpsGpggaMsg.gpsGpgga);
        nmea_zero_GPGSA(&gpsGpgsaMsg.gpsGpgsa);
        nmea_zero_GPGSV(&gpsGpgsvMsg.gpsGpgsv);
        nmea_zero_GPRMC(&gpsGprmcMsg.gpsGprmc);
        nmea_zero_GPVTG(&gpsGpvtgMsg.gpsGpvtg);

        CFE_ES_PerfLogExit(GPS_READER_PERF_ID);

        /* Wait on the 1Hz message (the timeout is 1 sec anyway) */
        int32 status = CFE_SB_ReceiveBuffer(&rcvMsgPtr, rcvPipe, 1000);

        CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t msgId = CFE_SB_GetMsgId(&rcvMsgPtr->Msg);
            CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG,
                              "GPS_READER: Got a message, id 0x%X", msgId);
        }

        /* Read from serial port */
        int32 nbytes = fill_serial_buffer(fd, serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE);
        CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "Read %d bytes",
                nbytes);

        /* Failed to read anything -> skip further processing this loop */
        if (nbytes < 0)
        {
            CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                "No bytes read! Trying to repoen serial port %s...",
                SERIAL_PORT_NAME);
            fd = try_open(SERIAL_PORT_NAME);
        }
        else
        {
            /* Parse input from serial port */
            int32 nmessages = nmea_parse(&gpsParser, serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE,
                    &gpsInfoMsg.gpsInfo);

            /* No messages found in buffer -> skip further processing */
            if (nmessages < 0)
            {
                CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                        "No NMEA GPS messages parsed!");
                continue;
            }

            // OS_printf("%s\n", serialBuffer);

            CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_INFORMATION,
                    "%d gps messages", nmessages);

            CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_INFORMATION,
                    "%d/%d sats inuse/inview",
                    gpsInfoMsg.gpsInfo.satinfo.inuse,
                    gpsInfoMsg.gpsInfo.satinfo.inview);


            /* A nmeaINFO messages is always created if any message is parsed */
            CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have info");
            CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsInfoMsg);
            CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsInfoMsg);

            /* Check which other messages were recieved */
            if (gpsInfoMsg.gpsInfo.smask & GPGGA)
            {
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have gpgga");
                nmea_info2GPGGA(&gpsInfoMsg.gpsInfo, &gpsGpggaMsg.gpsGpgga);
                CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpggaMsg);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpggaMsg);
            }

            if (gpsInfoMsg.gpsInfo.smask & GPGSA)
            {
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have gpgsa");
                nmea_info2GPGSA(&gpsInfoMsg.gpsInfo, &gpsGpgsaMsg.gpsGpgsa);
                CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpgsaMsg);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpgsaMsg);
            }

            if (gpsInfoMsg.gpsInfo.smask & GPGSV)
            {
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have gpgsv");
                nmea_info2GPGSV(&gpsInfoMsg.gpsInfo, &gpsGpgsvMsg.gpsGpgsv, 0);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpgsvMsg);
            }

            if (gpsInfoMsg.gpsInfo.smask & GPRMC)
            {
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have gprmc");
                nmea_info2GPRMC(&gpsInfoMsg.gpsInfo, &gpsGprmcMsg.gpsGprmc);
                CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGprmcMsg);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGprmcMsg);
            }

            if (gpsInfoMsg.gpsInfo.smask & GPVTG)
            {
                CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, CFE_EVS_EventType_DEBUG, "have gpvtg");
                nmea_info2GPVTG(&gpsInfoMsg.gpsInfo, &gpsGpvtgMsg.gpsGpvtg);
                CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &gpsGpvtgMsg);
                CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &gpsGpvtgMsg);
            }

        }
    }
}

void GPS_READER_Init(void)
{
    /* Initialize the EVS and ES */
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);

    /* Announce that the process is alive */
    CFE_EVS_SendEvent(GPS_READER_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                        "Startup. Version %d.%d.%d.%d",
                        GPS_READER_MAJOR_VERSION, GPS_READER_MINOR_VERSION,
                        GPS_READER_REVISION,      GPS_READER_MISSION_REV);

    /* Input pipe */
    CFE_SB_CreatePipe(&rcvPipe, 10, "GPS_READER_PIPE");
    /* 1 Hz message for the process tick rate */
    CFE_SB_Subscribe(GPS_READER_WAKEUP_MID, rcvPipe);

    /* Parser */
    nmea_parser_init(&gpsParser);

    /* Messages */
    CFE_SB_InitMsg(&gpsInfoMsg,  GPS_READER_GPS_INFO_MSG,  sizeof(gpsInfoMsg),  true);
    CFE_SB_InitMsg(&gpsGpggaMsg, GPS_READER_GPS_GPGGA_MSG, sizeof(gpsGpggaMsg), true);
    CFE_SB_InitMsg(&gpsGpgsaMsg, GPS_READER_GPS_GPGSA_MSG, sizeof(gpsGpgsaMsg), true);
    CFE_SB_InitMsg(&gpsGpgsvMsg, GPS_READER_GPS_GPGSV_MSG, sizeof(gpsGpgsvMsg), true);
    CFE_SB_InitMsg(&gpsGprmcMsg, GPS_READER_GPS_GPRMC_MSG, sizeof(gpsGprmcMsg), true);
    CFE_SB_InitMsg(&gpsGpvtgMsg, GPS_READER_GPS_GPVTG_MSG, sizeof(gpsGpvtgMsg), true);
}
