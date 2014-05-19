#pragma once

#include "registers.h"

Registers *GetActiveProcessRegisters();
ProcessId ChooseProcessToActivate();
void ActivateProcess( ProcessId id );
