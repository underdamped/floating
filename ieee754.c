/* the interface functions, spelled out for each precision with
 * explicit constants for utmost clarity
 */

#include <stdint.h>
#include "floating.h"

uint32_t getSign32(uint32_t x)
{
    return (x >> 31) & 1;
}

uint32_t getSign64(uint64_t x)
{
    return (x >> 63) & 1;
}

uint32_t getExp32(uint32_t x)
{
    return (x >> 23) & 0xFF; // 8 exponent bits
}

uint32_t getExp64(uint64_t x)
{
    return (x >> 52) & 0x7FF; // 11 exponent bits
}

uint32_t getMantissa32(uint32_t x)
{
    return x & 0x7FFFFF; // mask out bottom 23 bits
}

uint64_t getMantissa64(uint64_t x)
{
    return x & 0xFFFFFFFFFFFFF; // mask out bottom 52 bits
}

void incrementBinary32(struct data *d)
{
    ++(d->d32.i);
}

void incrementBinary64(struct data *d)
{
    ++(d->d64.i);
}

void decrementBinary32(struct data *d)
{
    --(d->d32.i);
}

void decrementBinary64(struct data *d)
{
    --(d->d64.i);
}
