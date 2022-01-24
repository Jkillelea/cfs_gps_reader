#include "gps_reader.h"
#include "cfe_es_extern_typedefs.h"
#include "cfe_psp.h"
#include "common_types.h"
#include "cfe.h"
#include "cfe_sb.h"
#include "cfe_evs.h"
#include "cfe_time.h"
#include "gps_reader_serial.h"

CFE_Status_t GPS_READER_RcvMsg(void)
{
    /* Init objects for this loop */
    CFE_PSP_MemSet( g_GPS_READER_Data.serialBuffer, 0, GPS_READER_SERIAL_BUFFER_SIZE);
    nmea_zero_INFO( &g_GPS_READER_Data.gpsInfoMsg.gpsInfo);
    nmea_zero_GPGGA(&g_GPS_READER_Data.gpsGpggaMsg.gpsGpgga);
    nmea_zero_GPGSA(&g_GPS_READER_Data.gpsGpgsaMsg.gpsGpgsa);
    nmea_zero_GPGSV(&g_GPS_READER_Data.gpsGpgsvMsg.gpsGpgsv);
    nmea_zero_GPRMC(&g_GPS_READER_Data.gpsGprmcMsg.gpsGprmc);
    nmea_zero_GPVTG(&g_GPS_READER_Data.gpsGpvtgMsg.gpsGpvtg);

    CFE_ES_PerfLogExit(GPS_READER_PERF_ID);

    /* Wait on the 1Hz message (the timeout is 1 sec anyway) */
    CFE_Status_t status = CFE_SB_ReceiveBuffer(&g_GPS_READER_Data.rcvMsgPtr,
                                                g_GPS_READER_Data.rcvPipe,
                                                CFE_SB_PEND_FOREVER);
                                                // 1000);

    CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

    if (status == CFE_SUCCESS)
    {
        CFE_SB_MsgId_t msgId = 0;
        if (CFE_MSG_GetMsgId(&g_GPS_READER_Data.rcvMsgPtr->Msg, &msgId) == CFE_SUCCESS)
        {
            // CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "GPS_READER: Got a message, id 0x%X", msgId);
        }
        else
        {
            // TODO: error
        }
    }

    /* Read from serial port */
    int32 nbytes = GPS_READER_ReadSerial(g_GPS_READER_Data.serialFd, g_GPS_READER_Data.serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE);

    /* Failed to read anything -> skip further processing this loop */
    if (nbytes < 0)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                "No bytes read! Trying to repoen serial port %s...",
                SERIAL_PORT_NAME);
        g_GPS_READER_Data.serialFd = GPS_READER_OpenPort(SERIAL_PORT_NAME);
    }
    else
    {
        /* Parse input from serial port */
        int32 nmessages = nmea_parse(&g_GPS_READER_Data.gpsParser, g_GPS_READER_Data.serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE,
                &g_GPS_READER_Data.gpsInfoMsg.gpsInfo);

        /* No messages found in buffer -> skip further processing */
        if (nmessages < 0)
        {
            CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                    "No NMEA GPS messages parsed!");
        }

        CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG,
                "%d gps messages", nmessages);

        CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG,
                "%d/%d sats inuse/inview",
                g_GPS_READER_Data.gpsInfoMsg.gpsInfo.satinfo.inuse,
                g_GPS_READER_Data.gpsInfoMsg.gpsInfo.satinfo.inview);


        /* A nmeaINFO messages is always created if any message is parsed */
        CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have info");
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsInfoMsg);
        CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsInfoMsg, true);

        /* Check which other messages were recieved */
        if (g_GPS_READER_Data.gpsInfoMsg.gpsInfo.smask & GPGGA)
        {
            CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have gpgga");
            nmea_info2GPGGA(&g_GPS_READER_Data.gpsInfoMsg.gpsInfo, &g_GPS_READER_Data.gpsGpggaMsg.gpsGpgga);
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpggaMsg);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpggaMsg, true);
        }

        if (g_GPS_READER_Data.gpsInfoMsg.gpsInfo.smask & GPGSA)
        {
            CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have gpgsa");
            nmea_info2GPGSA(&g_GPS_READER_Data.gpsInfoMsg.gpsInfo, &g_GPS_READER_Data.gpsGpgsaMsg.gpsGpgsa);
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpgsaMsg);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpgsaMsg, true);
        }

        if (g_GPS_READER_Data.gpsInfoMsg.gpsInfo.smask & GPGSV)
        {
            CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have gpgsv");
            nmea_info2GPGSV(&g_GPS_READER_Data.gpsInfoMsg.gpsInfo, &g_GPS_READER_Data.gpsGpgsvMsg.gpsGpgsv, 0);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpgsvMsg, true);
        }

        if (g_GPS_READER_Data.gpsInfoMsg.gpsInfo.smask & GPRMC)
        {
            CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have gprmc");
            nmea_info2GPRMC(&g_GPS_READER_Data.gpsInfoMsg.gpsInfo, &g_GPS_READER_Data.gpsGprmcMsg.gpsGprmc);
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGprmcMsg);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGprmcMsg, true);
        }

        if (g_GPS_READER_Data.gpsInfoMsg.gpsInfo.smask & GPVTG)
        {
            CFE_EVS_SendEvent(GPS_READER_INF_EID, CFE_EVS_EventType_DEBUG, "have gpvtg");
            nmea_info2GPVTG(&g_GPS_READER_Data.gpsInfoMsg.gpsInfo, &g_GPS_READER_Data.gpsGpvtgMsg.gpsGpvtg);
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpvtgMsg);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpvtgMsg, true);
        }
    }

    return status;
}

CFE_Status_t GPS_READER_Init(void)
{
    CFE_Status_t status = CFE_SUCCESS;
    CFE_PSP_MemSet(&g_GPS_READER_Data, 0, sizeof(g_GPS_READER_Data));

    /* Initialize the EVS and ES */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Failed to register with EVS");
        return status;
    }

    /* Input pipe */
    status = CFE_SB_CreatePipe(&g_GPS_READER_Data.rcvPipe, 10, "GPS_READER_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to create GPS_READER_PIPE");
        return status;
    }

    /* 1 Hz message for the process tick rate */
    CFE_SB_Subscribe(GPS_READER_WAKEUP_MID, g_GPS_READER_Data.rcvPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to subscribe to GPS_READER_WAKEUP_MID");
        return status;
    }

    /* Messages */
    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsInfoMsg,  GPS_READER_GPS_INFO_MSG,  sizeof(g_GPS_READER_Data.gpsInfoMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpggaMsg, GPS_READER_GPS_GPGGA_MSG, sizeof(g_GPS_READER_Data.gpsGpggaMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpgsaMsg, GPS_READER_GPS_GPGSA_MSG, sizeof(g_GPS_READER_Data.gpsGpgsaMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpgsvMsg, GPS_READER_GPS_GPGSV_MSG, sizeof(g_GPS_READER_Data.gpsGpgsvMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGprmcMsg, GPS_READER_GPS_GPRMC_MSG, sizeof(g_GPS_READER_Data.gpsGprmcMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    status = CFE_MSG_Init((CFE_MSG_Message_t *) &g_GPS_READER_Data.gpsGpvtgMsg, GPS_READER_GPS_GPVTG_MSG, sizeof(g_GPS_READER_Data.gpsGpvtgMsg));
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init message!");
        return status;
    }

    /* Parser */
    int nmeaStatus = nmea_parser_init(&g_GPS_READER_Data.gpsParser);
    if (nmeaStatus != 1)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR, "Failed to init NMEA parser!");
    }

    /* Try and open the serial port */
    g_GPS_READER_Data.serialFd = GPS_READER_OpenPort(SERIAL_PORT_NAME);
    if (g_GPS_READER_Data.serialFd < 0)
    {
        CFE_EVS_SendEvent(GPS_READER_ERROR_LOGMSG, CFE_EVS_EventType_ERROR,
                        "Failed to open serial port %s", SERIAL_PORT_NAME);
    }


    /* Announce that the process is alive */
    CFE_EVS_SendEvent(GPS_READER_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                        "Succesful Startup. Version %d.%d.%d.%d",
                        GPS_READER_MAJOR_VERSION, GPS_READER_MINOR_VERSION,
                        GPS_READER_REVISION,      GPS_READER_MISSION_REV);

    return status;
}

void GPS_READER_AppMain(void)
{
    CFE_Status_t status = CFE_SUCCESS;
    enum CFE_ES_RunStatus runStatus = CFE_ES_RunStatus_APP_RUN;

    CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);

    status = GPS_READER_Init();
    if (status != CFE_SUCCESS)
    {
        runStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    while (CFE_ES_RunLoop(&runStatus))
    {
        status = GPS_READER_RcvMsg();
        if (status != CFE_SUCCESS)
        {
            runStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    return;
}

