#ifndef _GPS_READER_PRINT_MSGS_H_
#define _GPS_READER_PRINT_MSGS_H_

// #include <stdio.h>
#include "cfe.h"
#include "nmea/nmea.h"

void print_info(nmeaINFO *info);
void print_gpgga(nmeaGPGGA *gpgga);
void print_gpgsa(nmeaGPGSA *gpgsa);
void print_gpgsv(nmeaGPGSV *gpgsv);
void print_gpvtg(nmeaGPVTG *gpvtg);
void print_gprmc(nmeaGPRMC *gpvtg);

#endif // _GPS_READER_PRINT_MSGS_H_
