#pragma once

extern ProcessId CreateProcess( char *pExecutablePath, int parameter );
extern Process *GetActiveProcess();
extern ProcessId ChooseProcessToActivate();
extern void ActivateProcess( ProcessId id );
