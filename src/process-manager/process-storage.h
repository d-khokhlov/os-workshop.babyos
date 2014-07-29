#pragma once

#include "process.h"

#define ProcessStorage_Capacity 100
typedef Process ProcessStorage[ ProcessStorage_Capacity ];

extern void ProcessStorage_Init( ProcessStorage storage );
