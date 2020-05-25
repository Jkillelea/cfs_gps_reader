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

void GPS_READER_Main(void);
void GPS_READER_Init(void);

#endif // _GPS_READER_H_
