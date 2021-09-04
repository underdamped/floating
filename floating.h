#pragma once

enum IEEE754_TYPE {
    binary32,
    binary64
};

// single-precision storage
union data32 {
    float f;
    uint32_t i;
};

// double-precision storage
union data64 {
    double f;
    uint64_t i;
};

// container for the containers and meta data
struct data {
    long double d128; // input value stored here

    union data32 d32;
    union data64 d64;

    enum IEEE754_TYPE floatType;
};

// floating.c and display.c
void showBinary32(struct data *);
void showBinary64(struct data *);
void showBits32(int);
void showBits64(int64_t);

// floating.c and ieee754.c
void incrementBinary32(struct data *);
void decrementBinary32(struct data *);
void incrementBinary64(struct data *);
void decrementBinary64(struct data *);

// floating.c and display.c and ieee754.c
uint32_t getSign32(uint32_t);
uint32_t getSign64(uint64_t);
uint32_t getExp32(uint32_t);
uint32_t getExp64(uint64_t);
uint32_t getMantissa32(uint32_t);
uint64_t getMantissa64(uint64_t);
