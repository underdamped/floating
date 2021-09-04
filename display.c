/* functions that do the pretty-printing */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "floating.h"

// ANSI color codes
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define CYN "\e[0;36m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BBLU "\e[1;34m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"
#define COLOR_RESET "\e[0m"

// mantissa scale factors for binary32 (23 mantissa bits) and binary64 (52 bits)
const uint32_t mantissaMax32 = 1U << 23;  // 2^23 = 0x800000
const uint64_t mantissaMax64 = 1UL << 52; // 2^52 = 0x10000000000000

// initialized in floating.c
extern bool wantInteger;
extern bool wantIncrement;
extern bool wantDecrement;

void showBinary32(struct data *data)
{
    // integer inputs (bit strings), increments, and decrements are by definition always exact
    if ( (fabsl(data->d128 - data->d32.f) > 0) && !wantInteger && !wantIncrement && !wantDecrement )
        printf( "[" BRED "inexact binary32 representation" COLOR_RESET "]\n" );
    else
        printf( "[" BGRN "exact binary32 representation" COLOR_RESET "]\n" );

    bool subnormal = false;
    uint8_t exp_raw = getExp32(data->d32.i);
    int8_t exp_unbiased = exp_raw ? exp_raw - 127 : -126; // subnormals are in range 2^-126
    uint32_t mantissa_raw = getMantissa32(data->d32.i);
    float mantissa_value = (float)mantissa_raw / mantissaMax32;

    if ( exp_raw == 0 )
    {
        subnormal = 1;
        printf( "[" RED "subnormal" COLOR_RESET "]\n" );
    }
    else if ( exp_raw == 255 && mantissa_raw != 0 )
    {
        printf( BRED "*** NaN\n" COLOR_RESET );
    }

    printf( "   float: " BWHT "%0.27g " COLOR_RESET "(%a)\n", data->d32.f, data->d32.f );
    printf( " integer: " BCYN "0x%.4X\n" COLOR_RESET, data->d32.i );
    printf( "    bits: " );  showBits32(data->d32.i);
    printf( "    sign: " BBLU "%d\n" COLOR_RESET, getSign32(data->d32.i) );
    printf( "exponent: " YEL "%d " COLOR_RESET "(0x%.2X) ", exp_raw, exp_raw );

    if ( exp_raw < 255 )
        printf( "[unbiased: 2^%d]\n", exp_unbiased );
    else
        printf( "\n" );

    printf( "mantissa: " CYN "0x%X " COLOR_RESET "[m/2^23 = %0.17g]\n", mantissa_raw, mantissa_value );

    if ( subnormal )
        printf( " formula: " GRN "0x%X * 2^-23 * 2^%d " COLOR_RESET "= %0.27g\n", mantissa_raw, exp_unbiased, data->d32.f );
    else
        printf( " formula: " GRN "(1 + %0.17g) * 2^%d " COLOR_RESET "= %0.27g\n", mantissa_value, exp_unbiased, data->d32.f );
}

void showBinary64(struct data *data)
{
    // integer inputs (bit strings), increments, and decrements are by definition always exact
    if ( (fabsl(data->d128 - data->d64.f) > 0) && !wantInteger && !wantIncrement && !wantDecrement )
        printf( "[" BRED "inexact binary64 representation" COLOR_RESET "]\n" );
    else
        printf( "[" BGRN "exact binary64 representation" COLOR_RESET "]\n" );

    bool subnormal = false;
    uint16_t exp_raw = getExp64(data->d64.i);
    int16_t exp_unbiased = exp_raw ? exp_raw - 1023 : -1022; // subnormals are in range 2^-1022
    uint64_t mantissa_raw = getMantissa64(data->d64.i);
    double mantissa_value = (double)mantissa_raw / mantissaMax64;

    printf( "   float: " BWHT "%0.27g " COLOR_RESET "(%a)\n", data->d64.f, data->d64.f );
    printf( " integer: " BCYN "0x%lX\n" COLOR_RESET, data->d64.i );
    printf( "    bits: " ); showBits64(data->d64.i);
    printf( "    sign: " BBLU "%d\n" COLOR_RESET, getSign64(data->d64.i) );
    printf( "exponent: " YEL "%d " COLOR_RESET "(0x%.2X) ", exp_raw, exp_raw );

    if ( exp_raw < 2047 )
        printf( "[unbiased: 2^%d]\n", exp_unbiased );
    else
        printf( "\n" );

    printf( "mantissa: " CYN "0x%lX " COLOR_RESET "[m/2^52 = %0.27g]\n", mantissa_raw, mantissa_value );

    if ( subnormal )
        printf( " formula: " GRN "0x%lX * 2^-52 * 2^%d " COLOR_RESET "= %0.27g\n", mantissa_raw, exp_unbiased, data->d64.f );
    else
        printf( " formula: " GRN "(1 + %0.27g) * 2^%d " COLOR_RESET "= %0.27g\n", mantissa_value, exp_unbiased, data->d64.f );
}

void showBits32(const int x)
{
    for ( int i = 31; i > 0; --i )
    {
        char *c;

        if ( (i != 31) && (31 - i) % 4 == 0 ) printf( " " );

        // color-code the sign, exponent, and mantissa bits
        if ( i == 31 ) c = BBLU;
        else if ( i < 31 && i > 22 ) c = YEL;
        else c = CYN;

        printf( "%s%d" COLOR_RESET, c, (x >> i) & 1 );
    }

    printf( CYN "%d\n" COLOR_RESET, x & 1 );
}

void showBits64(const int64_t x)
{
    for ( int i = 63; i > 0; --i )
    {
        char *c;

        if ( (i != 63) && (63 - i) % 4 == 0 ) printf( " " );

        // color-code the sign, exponent, and mantissa bits
        if ( i == 63 ) c = BBLU;
        else if ( i < 63 && i > 51 ) c = YEL; // 11 exponent bits, 52 through 62
        else c = CYN;

        printf( "%s%ld" COLOR_RESET, c, (x >> i) & 1 );
    }

    printf( CYN "%ld\n" COLOR_RESET, x & 1 );
}

void setBit32(int n, struct data *d)
{
    d->d32.i |= (1U << n);
}
