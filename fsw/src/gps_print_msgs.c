#include "gps_print_msgs.h"
#include "gps_reader_util.h"

void print_info(nmeaINFO *info) {
    OS_printf("info {\n");
    OS_printf("    mask = %d,\n", info->smask);
    OS_printf("    utc = {\n");
    OS_printf("        year = %d,\n", info->utc.year);
    OS_printf("        mon  = %d,\n", info->utc.mon);
    OS_printf("        day  = %d,\n", info->utc.day);
    OS_printf("        hour = %d,\n", info->utc.hour);
    OS_printf("        min  = %d,\n", info->utc.min);
    OS_printf("        sec  = %d,\n", info->utc.sec);
    OS_printf("        hsec = %d,\n", info->utc.hsec);
    OS_printf("    }\n");
    OS_printf("    sig         = %d,\n",  info->sig);
    OS_printf("    fix         = %d,\n",  info->fix);
    OS_printf("    PDOP        = %lf,\n", info->PDOP);
    OS_printf("    HDOP        = %lf,\n", info->HDOP);
    OS_printf("    VDOP        = %lf,\n", info->VDOP);
    OS_printf("    lat         = %lf,\n", decimal_minutes2decimal_decimal(info->lat));
    OS_printf("    lon         = %lf,\n", decimal_minutes2decimal_decimal(info->lon));
    OS_printf("    elv         = %lf,\n", info->elv);
    OS_printf("    speed       = %lf,\n", info->speed);
    OS_printf("    direction   = %lf,\n", info->direction);
    OS_printf("    declination = %lf,\n", info->declination);
    OS_printf("};\n");
}

void print_gpgga(nmeaGPGGA *gpgga) {
    OS_printf("GPGGA {\n");
    OS_printf("    utc = {\n");
    OS_printf("        year = %d,\n", gpgga->utc.year);
    OS_printf("        mon  = %d,\n", gpgga->utc.mon);
    OS_printf("        day  = %d,\n", gpgga->utc.day);
    OS_printf("        hour = %d,\n", gpgga->utc.hour);
    OS_printf("        min  = %d,\n", gpgga->utc.min);
    OS_printf("        sec  = %d,\n", gpgga->utc.sec);
    OS_printf("        hsec = %d,\n", gpgga->utc.hsec);
    OS_printf("    },\n");
    OS_printf("    lat      = %lf %s,\n", decimal_minutes2decimal_decimal(gpgga->lat), &gpgga->ns);
    OS_printf("    lon      = %lf %s,\n", decimal_minutes2decimal_decimal(gpgga->lon), &gpgga->ew);
    OS_printf("    sig      = %d,\n",     gpgga->sig);
    OS_printf("    satinuse = %d,\n",     gpgga->satinuse);
    OS_printf("    HDOP     = %lf,\n",    gpgga->HDOP);
    OS_printf("    elv      = %lf %s,\n", gpgga->elv,  &gpgga->elv_units);
    OS_printf("    diff     = %lf %s,\n", gpgga->diff, &gpgga->diff_units);
    OS_printf("    dgps_age = %lf,\n",    gpgga->dgps_age);
    OS_printf("    dgps_sid = %d,\n",     gpgga->dgps_sid);
    OS_printf("};\n");
}

void print_gpgsa(nmeaGPGSA *gpgsa) {
    OS_printf("GPGSA {\n");
    OS_printf("    fix_mode = %s,\n", &gpgsa->fix_mode);
    OS_printf("    fix_type = %d,\n", gpgsa->fix_type);
    // int i = 0;
    // while (gpgsa->sat_prn[i]) {
    //     OS_printf("    sat_prn = %d,\n", gpgsa->sat_prn[i]);
    //     i++;
    // }
    OS_printf("    PDOP = %lf,\n", gpgsa->PDOP);
    OS_printf("    HDOP = %lf,\n", gpgsa->HDOP);
    OS_printf("    VDOP = %lf,\n", gpgsa->VDOP);
    OS_printf("};\n");
}

void print_gpgsv(nmeaGPGSV *gpgsv) {
    OS_printf("GPGSV {\n");
    OS_printf("    pack_count = %d\n", gpgsv->pack_count);
    OS_printf("    pack_index = %d\n", gpgsv->pack_index);
    OS_printf("    sat_count  = %d\n", gpgsv->sat_count);
    // for (int i = 0; i < gpgsv->sat_count; i++) {
    //     nmeaSATELLITE sat = gpgsv->sat_data[i];
    //     OS_printf("    satellite = {\n");
    //     OS_printf("        id      = %d\n", sat.id);
    //     OS_printf("        in_use  = %d\n", sat.in_use);
    //     OS_printf("        elv     = %d\n", sat.elv);
    //     OS_printf("        azimuth = %d\n", sat.azimuth);
    //     OS_printf("        sig     = %d\n", sat.sig);
    //     OS_printf("    }\n");
    // }
    OS_printf("};\n");
}

void print_gpvtg(nmeaGPVTG *gpvtg) {
    OS_printf("GPVTG {\n");
    OS_printf("    dir = %lf %s\n", gpvtg->dir, &gpvtg->dir_t);
    OS_printf("    dec = %lf %s\n", gpvtg->dec, &gpvtg->dec_m);
    OS_printf("    spn = %lf %s\n", gpvtg->spn, &gpvtg->spn_n);
    OS_printf("    spk = %lf %s\n", gpvtg->spk, &gpvtg->spk_k);
    OS_printf("};\n");
}

void print_gprmc(nmeaGPRMC *gprmc) {
    OS_printf("GPRMC {\n");
    OS_printf("    utc = {\n");
    OS_printf("        year = %d,\n", gprmc->utc.year);
    OS_printf("        mon  = %d,\n", gprmc->utc.mon);
    OS_printf("        day  = %d,\n", gprmc->utc.day);
    OS_printf("        hour = %d,\n", gprmc->utc.hour);
    OS_printf("        min  = %d,\n", gprmc->utc.min);
    OS_printf("        sec  = %d,\n", gprmc->utc.sec);
    OS_printf("        hsec = %d,\n", gprmc->utc.hsec);
    OS_printf("    }\n");
    OS_printf("    status      = %s\n",    &gprmc->status);
    OS_printf("    lat         = %lf %s\n", decimal_minutes2decimal_decimal(gprmc->lat), &gprmc->ns);
    OS_printf("    lon         = %lf %s\n", decimal_minutes2decimal_decimal(gprmc->lat), &gprmc->ew);
    OS_printf("    speed       = %lf\n",    gprmc->speed);
    OS_printf("    direction   = %lf\n",    gprmc->direction);
    OS_printf("    declination = %lf %s\n", gprmc->declination, &gprmc->declin_ew);
    OS_printf("    mode        = %s\n",    &gprmc->mode);
    OS_printf("};\n");
}

