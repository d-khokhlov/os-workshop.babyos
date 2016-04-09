#pragma once

#include "process.h"

extern Process *CreateProcess( char *pExecutablePath, int parameter );
extern void TerminateProcess();
