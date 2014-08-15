#pragma once

// todo: вынести в более общий заголовочный файл
typedef void *Handle;

extern Handle CreateProcess( char *pExecutablePath, int parameter );
extern void TerminateProcess();
