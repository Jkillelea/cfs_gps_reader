#include "cfe.h"
#include "gps_reader_util.h"
#include "gps_reader_platform_cfg.h"

// convert from DDDMM.mmmmm (decimal minutes) to DDD.dddddd (plain decimal) format
// TODO: double check this!
double decimal_minutes2decimal_decimal(const double decimal_minutes) {
    double degrees = ((int) (decimal_minutes/100.0)); // DDD
    double minutes = decimal_minutes - 100*degrees;   // MM.mmmmmm
    double decimal = minutes / 60;                    // 0.ddddddd
    return (degrees + decimal);                       // DDD.dddddd
}

