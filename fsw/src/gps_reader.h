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

#include "sc_msgids.h"

#define GPS_READER_SERIAL_BUFFER_SIZE (512)

void GPS_READER_Main(void);
void GPS_READER_Init(void);

#endif // _GPS_READER_H_
