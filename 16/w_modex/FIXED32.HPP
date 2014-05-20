#ifndef FIXEDPOINT_HPP
    #define FIXEDPOINT_HPP

typedef long Fixed32;          // 16.16 FixedPoint
typedef unsigned char Iangle;  // Integer angle (0..255)

/* Macros for Type Conversion */
#define INT_TO_FIXED(x) ((x) << 16)
#define FIXED_TO_INT(x) ((x) >> 16)
#define ROUND_FIXED_TO_INT(x) (((x) + 0x8000) >> 16)

// Loads Fixed32 datafiles
void initFixed32(void);

// Common math functions
Fixed32 FixedMul(Fixed32 num1, Fixed32 num2);
Fixed32 FixedDiv(Fixed32 numer, Fixed32 denom);
void CosSin(Iangle theta, Fixed32 *Cos, Fixed32 *Sin);

Fixed32 FixedMulASM(Fixed32 num1, Fixed32 num2);
#pragma aux FixedMulASM =   \
    "imul edx"              \
    "add eax, 8000h"        \
    "adc edx, 0"            \
    "shrd eax, edx, 16"     \
    parm caller [eax] [edx] \
    value [eax]             \
    modify [eax edx];

Fixed32 FixedDivASM(Fixed32 numer, Fixed32 denom);  // No rounding!
#pragma aux FixedDivASM =   \
    "xor eax, eax"          \
    "shrd eax, edx, 16"     \
    "sar edx, 16"           \
    "idiv ebx"              \
    parm caller [edx] [ebx] \
    value [eax]             \
    modify [eax ebx edx];

#endif

