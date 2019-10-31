#include "cfe.h"
#include "gps_reader_util.h"
#include "gps_reader_platform_cfg.h"

int try_close(int fd) {
    if (fd > 0)
        return close(fd);
    return -1;
}

int set_serialport_params(int fd) {
    struct termios serialport;
    memset(&serialport, 0, sizeof(struct termios));

    // begin gross POSIX serial port code
    if (tcgetattr(fd, &serialport) != 0) {
        perror("Failed to get attrs");
        return -1;
    }

    cfsetispeed(&serialport, SPEED);
    cfsetospeed(&serialport, SPEED);

    serialport.c_cflag = (serialport.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    serialport.c_iflag &= ~IGNBRK; // disable break processing
    serialport.c_lflag = 0;        // no signaling chars, no echo,
                                   // no canonical processing
    serialport.c_oflag = 0;        // no remapping, no delays
    serialport.c_cc[VMIN]  = 1;    // read blocks
    serialport.c_cc[VTIME] = 50;   // 5 seconds read timeout

    serialport.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    serialport.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls,
                                                   // enable reading
    serialport.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    serialport.c_cflag |=  PARITY;
    serialport.c_cflag &= ~CSTOPB;
    // serialport.c_cflag &= ~CRTSCTS; // vs code doesn't find this one flag? still compiles.

    // From the manpages. Sets it to raw mode. Otherwise linux can postprocess
    // the \r\n into \n\n and the parser fails
    serialport.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                            | INLCR | IGNCR  | ICRNL  | IXON);
    serialport.c_oflag &= ~OPOST;
    serialport.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    serialport.c_cflag &= ~(CSIZE | PARENB);
    serialport.c_cflag |= CS8;

    if (tcsetattr(fd, TCSANOW, &serialport) != 0) {
        perror("tcsetattr failed");
        return -4;
    }

    return 0;
}

int try_open(const char *portname) {
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
        return -1;

    if (set_serialport_params(fd) < 0) {
        return -2;
    }
    return fd;
}

// convert from DDDMM.mmmmm (decimal minutes) to DDD.dddddd (plain decimal) format
// TODO: double check this!
double decimal_minutes2decimal_decimal(const double decimal_minutes) {
    double degrees = ((int) (decimal_minutes/100.0)); // DDD
    double minutes = decimal_minutes - 100*degrees;   // MM.mmmmmm
    double decimal = minutes / 60;                    // 0.ddddddd
    return (degrees + decimal);                       // DDD.dddddd
}

// read until buffer is full
size_t fill_serial_buffer(int fd, char *serialBuffer, size_t len) {
        size_t recieved_bytes = 0; // total bytes read
        while(recieved_bytes < len) {
            size_t nbytes_read = read(fd,
                                    serialBuffer + recieved_bytes, // pointer plus offset
                                    len - recieved_bytes);         // buffer length remaining

            recieved_bytes += nbytes_read;

            if (nbytes_read == 0) { // failed to read anything?
                OS_printf("Failed to read from fd (0 bytes).");
                try_close(fd);
                fd = try_open(SERIAL_PORT_NAME);
                if (fd < 0) {
                    perror("Failed to reopen fd.");
                }
            }
        }

    return recieved_bytes;
}
