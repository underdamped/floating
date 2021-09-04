/*
 *       _|  |                |   _)               
 *      |    |   _ \    _` |  __|  |  __ \    _` | 
 *      __|  |  (   |  (   |  |    |  |   |  (   | 
 *     _|   _| \___/  \__,_| \__| _| _|  _| \__, | 
 *                                          |___/  
 *
 * an IEEE754 floating-point analysis tool
 *
 * Javier Lombillo <javier@asymptotic.org>
 * 2021-05-11
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include "floating.h"

static int parseArgs(const int, char **, struct data *);
static void initialize(struct data *);
static void showFacts(void);
static void showHelp(void);

// each of these will point to a corresponding function of the correct ieee754 type
static void (*showData)(struct data *);
static void (*incrementData)(struct data *);
static void (*decrementData)(struct data *);

const char *progname;

bool wantInteger = false;
bool wantIncrement = false;
bool wantDecrement = false;

int main(int argc, char **argv)
{
    char *str, *endptr;
    int valueidx;

    struct data data = { .floatType = binary32 };

    progname = argv[0];

    if ( sizeof(long double) == sizeof(double) )
        printf( "[Warning] long double has same precision as double, precision may be compromised.\n" );

    valueidx = parseArgs( argc, argv, &data );    

    str = argv[valueidx];
    errno = 0;

    data.d128 = strtold( str, &endptr );

    if ( (errno == ERANGE && (data.d128 == HUGE_VALF || data.d128 == HUGE_VALL)) || (errno != 0 && data.d128 == 0) )
    {
        perror("strtold");
        exit(EXIT_FAILURE);
    }

    if ( endptr == str )
    {
        fprintf(stderr, "Non-numeric input\n");
        exit(EXIT_FAILURE);
    }

    if ( *endptr != '\0' )
    {
        fprintf( stderr, "Error: didn't understand part of input: %s\n", endptr );
        exit(EXIT_FAILURE);
    }

    initialize(&data);   

    if      ( wantIncrement ) incrementData(&data);
    else if ( wantDecrement ) decrementData(&data);

    showData(&data);

    return 0;
}


static void initialize(struct data *d)
{
    switch (d->floatType)
    {
        case binary32:

            // set up the appropriate function calls
            showData = &showBinary32;
            incrementData = &incrementBinary32;
            decrementData = &decrementBinary32;

            // interpret the input
            if ( wantInteger )
                d->d32.i = (uint32_t)d->d128;
            else
                d->d32.f = (float)d->d128;

            break;

        case binary64:

            // set up the function pointers
            showData = &showBinary64;
            incrementData = &incrementBinary64;
            decrementData = &decrementBinary64;

            // interpret the input
            if ( wantInteger )
                d->d64.i = (uint64_t)d->d128;
            else
                d->d64.f = (double)d->d128;
            break;

        default:
            fprintf( stderr, "[Error] unknown format\n" );
            exit( EXIT_FAILURE );
    }
}

static int parseArgs(const int argc, char **argv, struct data *d)
{
    int opt;

    while ( (opt = getopt(argc, argv, "de:fhiMm:Ps:")) != -1 )
    {
        switch (opt)
        {
            case 'd': // analyze as double-precision float
                d->floatType = binary64;
                break;
            case 'i': // treat the input as an integer bit string
                wantInteger = true;
                break;
            case 'f': // amaze your friends!
                showFacts();
                break;
            case 'P': // show the next float
                wantIncrement = true;
                break;
            case 'M': // show the previous float
                wantDecrement = true;
                break;
            case 'h': // fall-through
            default:
                showHelp();
        }
    }

    // check for no args
    if ( optind == argc ) showHelp();

    return optind; // index of the first non-option argument
}

static void showFacts(void)
{
    printf( "Fun IEEE754 facts:\n\n" );
    printf( "[binary32] smallest normal positive value: %0.27g\n", FLT_MIN );
    printf( "[binary64] smallest normal positive value: %0.27g\n", DBL_MIN );
    printf( "[binary32] smallest subnormal positive value: %0.27g\n", FLT_TRUE_MIN );
    printf( "[binary64] smallest subnormal positive value: %0.27g\n", DBL_TRUE_MIN );
    puts("");
    printf( "[binary32] largest positive finite value: %0.27g\n", FLT_MAX );
    printf( "[binary64] largest positive finite value: %0.27g\n", DBL_MAX );
    puts("");
    printf( "[binary32] +Inf: all exponent bits set, mantissa all zeros, sign zero.\n" );
    printf( "[binary32] -Inf: all exponent bits set, mantissa all zeros, sign one.\n" );
    puts("");
    printf( "[binary32] NaN: all exponent bits set, at least one mantissa bit set.\n" );
    printf( "[binary32] there are 2^24 - 2 total NaN values, about 0.39%% of the binary32 space.\n" );
    puts("");
    printf( "[binary32] the number 134 (0x86) is the only non-zero float that equals its own exponent value.\n" );
    // i've written an elegant proof of the preceding theorem, but this comment is too short to show it

    exit( EXIT_SUCCESS );
}

static void showHelp(void)
{
    printf( "%s -- show binary representation of an IEEE754 floating-point number\n"
            "Usage:\n\n"
            "  %s [options] <number>\n\n", progname, progname );
    printf( "where <number> can be in decimal (3.14), hex (0x42), exponential (1.4E-12), or\n"
            "binary exponent (0x1.4p+1) form.\n\n" );
    printf( "Options:\n" );
    printf( "  -d      use double-precision binary64 format\n"
            "  -f      show a few interesting IEEE754 facts\n"
            "  -i      treat the input as an integer bit string, then interpret as float\n"
            "  -P      increment the given number to the next floating-point value (Plus)\n"
            "  -M      decrement the given number to the previous floating-point value (Minus)\n"
            "\nNB: to enter a negative value, use '--' to stop processing arguments, e.g.,\n"
            "  %s -d -- -3.14\n", progname
        );

    exit( EXIT_FAILURE );
}
