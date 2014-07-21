#pragma once

extern Process *GetActiveProcess();
extern ProcessId ChooseProcessToActivate();
extern void ActivateProcess( ProcessId id );
