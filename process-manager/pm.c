//#include <stdio.h>

void execute( char *executableName ) {

    unsigned long newStackAddress = 0;
    unsigned long newCodeAddress = 0;

    __asm {

        // Открыть исполняемый файл на чтение
        mov     ah, 0x3D
        mov     al, 10100000b
        mov     dx, executableName
        mov     cl, 0x00
        int     21h

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

        // Инициализировать стек для загружаемого процесса
        mov     word ptr newStackAddress, 0xFFFF
        mov     word ptr newStackAddress + 2, ds
        mov     dx, sp
        lss     sp, newStackAddress

        // Сохранить в стек загружаемого процесса указатель стека текущего процесса
        push    cs
        push    dx

        // Загрузить файл в целевой сегмент
        mov     ah, 0x3F
        mov     cx, 0xFFFF
        mov     dx, 0x0000
        int     21h

        // Закрыть файл
        mov     ah, 0x3E
        int     21h

        // Передать управление загруженному процессу
        mov     word ptr newCodeAddress, 0x0000
        mov     word ptr newCodeAddress + 2, ds
        call    dword ptr newCodeAddress

    @restore:

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

void main( int argc, char **argv ) {
	//printf("%s\n", argv[ 1 ] );
    //execute( argv[ 1 ] );
    execute( "hw");
}
