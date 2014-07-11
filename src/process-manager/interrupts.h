#pragma once

#include "common.h"

extern void asmcall SetInterruptHandler( unsigned short interrupt, void far *pHandler );
extern void far * asmcall GetInterruptHandler( unsigned short interrupt );
