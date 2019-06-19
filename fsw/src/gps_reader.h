#ifndef _gps_reader_h_
#define _gps_reader_h_

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

#include "sc_msgids.h"

#define GPS_READER_SERIAL_BUFFER_SIZE (512)

void GPS_READER_Main(void);
void GPS_READER_Init(void);

#endif // _gps_reader_h_
