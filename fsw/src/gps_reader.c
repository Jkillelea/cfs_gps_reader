#include "gps_reader.h"
#include "gps_reader_constants.h"
#include "gps_reader_util.h"
#include "gps_print_msgs.h"
#include "nmea/nmea.h"

CFE_SB_PipeId_t commandPipe;
CFE_SB_MsgPtr_t msgPtr;

char serialBuffer[GPS_READER_SERIAL_BUFFER_SIZE];

void GPS_READER_Main(void) {
    uint32 runStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(GPS_READER_PERF_ID);
    
    GPS_READER_Init();

    int32 fd = try_open(portname);
    if (fd < 0) {
        OS_printf("GPS_READER: Failed to open serial port %s\n", portname);
    }

    nmeaPARSER gpsParser;
    nmeaINFO   gpsInfo;
    nmea_parser_init(&gpsParser);

    while (CFE_ES_RunLoop(&runStatus) == TRUE) {
        nmea_zero_INFO(&gpsInfo);

        int32 status = CFE_SB_RcvMsg(&msgPtr, commandPipe, CFE_SB_PEND_FOREVER);
        OS_printf("GPS_READER: Got a message\n");

        if (status == CFE_SUCCESS) {
            CFE_SB_MsgId_t msgId = CFE_SB_GetMsgId(msgPtr);
            int len = CFE_SB_GetTotalMsgLength(msgPtr);
            OS_printf("GPS_READER: msg id 0x%x (%d)\n", msgId, len);
        }

        // read from serial port
        fill_serial_buffer(fd, serialBuffer, GPS_READER_SERIAL_BUFFER_SIZE);

        int32 nmessages = nmea_parse(&gpsParser, 
                                    serialBuffer, 
                                    GPS_READER_SERIAL_BUFFER_SIZE,
                                    &gpsInfo);

        if (nmessages > 0) {
            // OS_printf("GPS_READER: %d gps messages\n", nmessages);
            print_info(&gpsInfo);
        }

        // CFE_EVS_SendEvent(GPS_READER_INFO_LOGMSG, 
        //                     CFE_EVS_INFORMATION, 
        //                     "\n%s\n", serialBuffer);
    }
    
}

void GPS_READER_Init(void) {
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    CFE_EVS_SendEvent(GPS_READER_STARTUP_INF_EID, 
                        CFE_EVS_INFORMATION, 
                        "GPS_READER: startup.");

    CFE_SB_CreatePipe(&commandPipe, 10, "GPS_READER_PIPE");
    CFE_SB_Subscribe(SC_1HZ_WAKEUP_MID, commandPipe);

    // CFE_SB_InitMsg(&msgPtr, SC_1HZ_WAKEUP_MID, )
}