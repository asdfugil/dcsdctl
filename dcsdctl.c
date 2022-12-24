#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <ftdi.h>
#include <unistd.h>
#include <ctype.h>

enum
{
    DCSD_NONE = 0xf4,
    DCSD_GREEN = 0xf2,
    DCSD_YELLOW = 0xf8,
    DCSD_RED = 0xf1,
    DCSD_ALL = 0xfb,
    DCSD_YELLOW_GREEN = 0xfa,
    DCSD_RED_YELLOW = 0xf9,
    DCSD_RED_GREEN = 0xf3,
};

struct Status
{
    uint8_t red : 1;
    uint8_t yellow : 1;
    uint8_t green : 1;
};

int light_up(uint8_t led)
{
    struct ftdi_context *ftdi;
    int fopen;
    unsigned char buf[1];
    ftdi = ftdi_new();

    if (ftdi == 0)
    {
        fprintf(stderr, "ftdi_new() failed.\n");
        return -1;
    }

    fopen = ftdi_usb_open(ftdi, 0x0403, 0x8a88);

    if (fopen < 0)
    {
        fprintf(stderr, "DCSD cable not found.\n");
        return 1;
    }

    fopen = ftdi_set_bitmode(ftdi, led, BITMODE_CBUS);

    if (fopen < 0)
    {
        ftdi_get_error_string(ftdi);
        ftdi_usb_close(ftdi);
        ftdi_free(ftdi);
        exit(-1);
    }
    fopen = ftdi_read_pins(ftdi, &buf[0]);

    if (fopen < 0)
    {
        ftdi_usb_close(ftdi);
        ftdi_free(ftdi);
        return -1;
    }
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);

    return 0;
}

void usage(char *argv0)
{
    fprintf(stderr, "usage: %s <status or led1> [[led2] [led3]...]\n\n", argv0);
    fprintf(stderr, "If <status or led> is an integer, then it will be interpreted\n");
    fprintf(stderr, "as the bitmask that's going to be sent to the dcsd cable, and\n");
    fprintf(stderr, "all other arguments are ignored. Else each argument will represent\n");
    fprintf(stderr, "which LED in the DCSD cable to light up, where:\n\n");
    fprintf(stderr, "red/fail = The red LED\n");
    fprintf(stderr, "yellow/busy = The yellow LED\n");
    fprintf(stderr, "green/pass = The green LED\n");
    fprintf(stderr, "all = All LEDs\n");
    fprintf(stderr, "none = No LEDs (all off)\n\n");
    fprintf(stderr, "If conflicting arguments are specified, then the later arguments\n");
    fprintf(stderr, "will override the previous ones.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    struct Status status;
    status.red = 0;
    status.green = 0;
    status.yellow = 0;
    if (atoi(argv[1]) != 0 || *argv[1] == '0')
    {
        return light_up(atoi(argv[1]));
    }
    for (int i = 1; argv[i] != NULL; i++)
    {
        if (!strcmp(argv[i], "red") || !strcmp(argv[i], "fail"))
        {
            status.red = 1;
        }
        else if (!strcmp(argv[i], "yellow") || !strcmp(argv[i], "busy"))
        {
            status.yellow = 1;
        }
        else if (!strcmp(argv[i], "green") || !strcmp(argv[i], "pass"))
        {
            status.green = 1;
        }
        else if (!strcmp(argv[i], "none"))
        {
            status.green = 0;
            status.yellow = 0;
            status.red = 0;
        }
        else if (!strcmp(argv[i], "all"))
        {
            status.green = 1;
            status.yellow = 1;
            status.red = 1;
        }
        else
        {
            fprintf(stderr, "Cannot parse argument %d \"%s\".\n", i, argv[i]);
            usage(argv[0]);
            return -1;
        }
    }
    if (!status.green && !status.yellow && !status.red)
    {
        return light_up(DCSD_NONE);
    }
    else if (status.green && !status.yellow && !status.red)
    {
        return light_up(DCSD_GREEN);
    }
    else if (!status.green && status.yellow && !status.red)
    {
        return light_up(DCSD_YELLOW);
    }
    else if (!status.green && !status.yellow && status.red)
    {
        return light_up(DCSD_RED);
    }
    else if (status.green && status.yellow && status.red)
    {
        return light_up(DCSD_ALL);
    }
    else if (status.green && status.yellow && !status.red)
    {
        return light_up(DCSD_YELLOW_GREEN);
    }
    else if (!status.green && status.yellow && status.red)
    {
        return light_up(DCSD_RED_YELLOW);
    }
    else if (status.green && !status.yellow && status.red)
    {
        return light_up(DCSD_RED_GREEN);
    }
}
