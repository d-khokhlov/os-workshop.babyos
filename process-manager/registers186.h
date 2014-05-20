#pragma once

typedef unsigned short CommonRegister;
typedef unsigned short SegmentRegister;
typedef unsigned short FlagsRegister;

typedef struct Registers
{
    CommonRegister ax, bx, cx, dx;
    CommonRegister si, di;
    CommonRegister bp, sp;
    FlagsRegister flags;
    SegmentRegister cs, ds, ss, es;
} Registers;
