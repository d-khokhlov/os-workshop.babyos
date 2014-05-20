#include "clock.h"
#include "process.h"
#include "registers.h"

#define _MAX_PROCESSES_COUNT 10
Process _processes[ _MAX_PROCESSES_COUNT ];
ProcessId _activeProcessId;

ProcessId CreateProcess( char *executablePath, int parameter )
{
}

ProcessId ChooseProcessToActivate()
{
    ProcessId nextId = _GetNextReadyProcess( _activeProcessId );
    if ( nextId == ProcessId_None ) {
        return nextId;
    } else {
        return _activeProcessId;
    }
}

void ActivateProcess( ProcessId id )
{
    if ( id != _activeProcessId ) {
        _processes[ _activeProcessId ].state = ProcessState_Ready;
        _processes[ id ].state = ProcessState_Active;
    }
}

Registers *GetActiveProcessRegisters()
{
    return &( _processes[ _activeProcessId ].registers );
}

void _Execute( char *executableName, int parameter )
{
    unsigned long newCodeAddress = 0;

    __asm {

        // Открыть исполняемый файл на чтение
        mov     ah, 0x3D;
        mov     al, 10100000b;
        mov     dx, executableName;
        mov     cl, 0x00;
        int     0x21;

        // Сохранить дескриптор открытого файла
        mov     bx, ax;

        // Вычислить сегментный адрес для загружаемого процесса
        mov     cx, ds:0x0002;
        sub     cx, 0x1000;

        // Инициализировать сегментные регистры данных для загружаемого процесса
        mov     ds, cx;
        mov     es, cx;

        // Сохранить указатель базы стекового кадра текущего процесса
        push    bp;

        // Забрать переданный параметр из стека, т.к. далее стек будет утерян
        mov     cx, parameter;

        // Запомнить указатель стека текущего процесса
        mov     dx, sp;

        // Инициализировать стек для загружаемого процесса
        mov     ax, ds;
        mov     ss, ax;
        mov     sp, 0xFFFE;

        // Сохранить в стек загружаемого процесса адрес стека текущего процесса
        push    dx;
        push    cs;

        // Передать параметр загружаемому процессу через его стек
        push    cx;

        // Загрузить файл в целевой сегмент
        mov     ah, 0x3F;
        mov     cx, 0xFFFF;
        mov     dx, 0x0000;
        int     0x21;

        // Закрыть файл
        mov     ah, 0x3E;
        int     0x21;

        // Передать управление загруженному процессу
        mov     word ptr newCodeAddress, 0x0000;
        mov     word ptr newCodeAddress + 2, ds;
        call    dword ptr newCodeAddress;

        // Восстановить стек текущего процесса
        pop     ax;
        pop     sp;
        mov     ss, ax;

        // Восстановить стековый кадр текущего процесса
        pop     bp;

        // Восстановить сегментные регистры текущего процесса
        mov     cx, cs;
        mov     ds, cx;
        mov     es, cx;
    }
}

ProcessId _GetNextReadyProcess( ProcessId id )
{

    ProcessId nextId = id;
    do {

        nextId++;
        if ( nextId >= _MAX_PROCESSES_COUNT ) {
            nextId = 0;
        }

        if ( _processes[ nextId ].state == ProcessState_Ready ) {
            return nextId;
        }

    } while ( nextId != id );

    return ProcessId_None;
}

void main()
{
    //int parameter = 0;
    //char filename = argv[ 1 ]
    //_Execute( argv[ 1 ] );
    //_Execute( "runstr", 2 );
    InitClock();
    __asm {
        mov     ah, 0x01
            int     0x21
    }
}
