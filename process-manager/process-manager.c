#include "clock.h"
#include "process.h"

#define MAX_PROCESSES_COUNT 10
Process g_processes[ MAX_PROCESSES_COUNT ];
ProcessId g_activeProcessId;

void execute( char *executableName, int parameter ) {

    unsigned long newStackAddress = 0;
    unsigned long newCodeAddress = 0;

    __asm {

        // Открыть исполняемый файл на чтение
        mov     ah, 0x3D
        mov     al, 10100000b
        mov     dx, executableName
        mov     cl, 0x00
        int     0x21

        // Сохранить дескриптор открытого файла
        mov     bx, ax

        // Вычислить сегментный адрес для загружаемого процесса
        mov     cx, ds:0x0002
        sub     cx, 0x1000

        // Инициализировать сегментные регистры данных для загружаемого процесса
        mov     ds, cx
        mov     es, cx
        mov     fs, cx
        mov     gs, cx

        // Сохранить указатель базы стекового кадра
        push    bp

        // Сохранить переданный параметр
        mov     cx, parameter

        // Инициализировать стек для загружаемого процесса
        mov     word ptr newStackAddress, 0xFFFE
        mov     word ptr newStackAddress + 2, ds
        mov     dx, sp
        lss     sp, newStackAddress

        // Сохранить в стек загружаемого процесса указатель стека текущего процесса
        push    cs
        push    dx

        // Передать параметр загружаемому процессу через его стек
        push    cx

        // Загрузить файл в целевой сегмент
        mov     ah, 0x3F
        mov     cx, 0xFFFF
        mov     dx, 0x0000
        int     0x21

        // Закрыть файл
        mov     ah, 0x3E
        int     0x21

        // Передать управление загруженному процессу
        mov     word ptr newCodeAddress, 0x0000
        mov     word ptr newCodeAddress + 2, ds
        call    dword ptr newCodeAddress

        // Восстановить стек текущего процесса
        mov     bp, sp
        lss     sp, dword ptr [ bp ]

        // Восстановить стековый кадр текущего процесса
        pop     bp

        // Восстановить сегментные регистры текущего процесса
        mov     cx, cs
        mov     ds, cx
        mov     es, cx
        mov     fs, cx
        mov     gs, cx

    }
}

ProcessId getNextReady( ProcessId id ) {

    ProcessId nextId = id;
    do {

        nextId++;
        if ( nextId >= MAX_PROCESSES_COUNT ) {
            nextId = 0;
        }

        if ( g_processes[ nextId ].state == PROCESS_STATE_READY ) {
            return nextId;
        }

    } while ( nextId != id );

    return PROCESS_ID_NONE;
}

ProcessId chooseProcessToActivate() {
    ProcessId nextId = getNextReady( g_activeProcessId );
    if ( nextId == PROCESS_ID_NONE ) {
        return nextId;
    } else {
        return g_activeProcessId;
    }
}

void activateProcess( int id ) {

}

void main( int argc, char **argv ) {
    //int parameter = 0;
    //char filename = argv[ 1 ]
    //execute( argv[ 1 ] );
    //execute( "runstr", 2 );
    initClock();
    __asm {
        mov     ah, 0x01
        int     0x21
    }
}
