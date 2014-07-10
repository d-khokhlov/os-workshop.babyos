#pragma once

typedef unsigned int CommonRegister;
typedef unsigned short SegmentRegister;
typedef unsigned int FlagsRegister;

typedef struct Registers
{
    CommonRegister ax, bx, cx, dx;
    CommonRegister si, di;
    CommonRegister bp, sp;
    FlagsRegister flags;
    SegmentRegister cs, ds, ss, es;
    CommonRegister ip;
} Registers;
