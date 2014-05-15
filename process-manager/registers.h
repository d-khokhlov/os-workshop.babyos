#pragma once

typedef struct {

    unsigned short ax, bx, cx, dx;
    unsigned short si, di;
    unsigned short bp, sp;
    unsigned short flags;
    unsigned short cs, ds, ss, es, fs, gs;

} Registers;
