                .model  tiny
                .186

                public  DEBUG_PrintState

                .code

_SCREEN_WIDTH \
                equ     80
_SCREEN_HEIGHT  equ     25
_TEXT_BUFFER_LINE_SIZE \
                equ     _SCREEN_WIDTH * 2
_TEXT_BUFFER_SIZE \
                equ     _TEXT_BUFFER_LINE_SIZE * _SCREEN_HEIGHT
_textBufferOffset \
                dw      0

_BULLET_COLOR \
                equ     0x0C
_REGISTERS_BACK_COLOR \
                equ     0x00
_FLAGS_BACK_COLOR \
                equ     0x10
_STACK_BACK_COLOR \
                equ     0x40
_LABEL_FORE_COLOR \
                equ     0x0A
_VALUE_FORE_COLOR_1 \
                equ     0x0F
_VALUE_FORE_COLOR_2 \
                equ     0x07
_SET_FLAG_FORE_COLOR \
                equ     0x0E
_CLEARED_FLAG_FORE_COLOR \
                equ     0x08
_bulletLabel    db      '>>', 0
_stackLabel     db      'stack', 0
_valueForeColors \
                db      _VALUE_FORE_COLOR_1, _VALUE_FORE_COLOR_2
_flagForeColors db      _CLEARED_FLAG_FORE_COLOR, _SET_FLAG_FORE_COLOR

_REGISTERS_COUNT \
                equ     13 ; не считаем регистр флагов
_PRINT_STACK_BYTES_COUNT \
                equ     32
; Таблица для хранения регистров. Начальные значения - смещения относительно
; вершины стека, по которым регистры попадут в стек
_registersTable dw      'ax', 22, ?, 'bx', 16, ?, 'cx', 20, ?, 'dx', 18, ?,
                        'si', 10, ?, 'di',  8, ?, 'bp', 12, ?,
                        'sp', 14
_registersTable_Sp \
                dw      ?,
                        'ds',  6, ?, 'es',  4, ?, 'ss',  2, ?, 'cs',  0, ?,
                        'ip', 26, ?
_flags          dw      'fl', 24, ?


; Нет ввода
; Нет вывода
DEBUG_PrintState:
                pushf
                pusha
                push    ds
                push    es
                push    ss ; для единообразия обработки
                push    cs ; для единообразия обработки

                mov     cx, _REGISTERS_COUNT + 1 ; флаги скопируем в общем цикле
                mov     si, offset _registersTable + 2
                mov     bp, sp
@saveRegister:
                mov     bx, [ si ]
                add     bx, bp
                mov     bx, [ ss:bx ]
                add     si, 2
                mov     [ si ], bx
                add     si, 4
                loop    @saveRegister
                add     _registersTable_Sp, 4 ; скорректировать sp

                mov     ax, 0xB800
                mov     es, ax
                mov     di, _textBufferOffset

                ; DEBUG
                jmp     @skipDebug
                mov     bh, 0x0F
                mov     cx, 0x0100
                mov     dx, 0x0000
@nextWord:
                mov     ax, dx
                call    _PrintWordHex
                mov     al, ' '
                call    _PrintByteChar
                add     dh, 2
                inc     dl
                loop    @nextWord
                mov     si, offset _stackLabel
                call    _PrintString
                mov     si, offset _stackLabel
                mov     byte ptr _stackLabel, 0
                call    _PrintString
                mov     si, offset _stackLabel
                mov     byte ptr _stackLabel, '-'
                call    _PrintString
                mov     ax, 'ax'
                call    _PrintWordChar
                mov     ax, 0x686C
                call    _PrintWordChar
@skipDebug:
                ; END DEBUG

                mov     bh, _BULLET_COLOR
                mov     si, offset _bulletLabel
                call    _PrintString
                call    _PrintRegisters
                call    _PrintFlags
                call    _PrintStack

                mov     ax, _textBufferOffset
@addLine:
                add     ax, _TEXT_BUFFER_LINE_SIZE
                cmp     ax, _TEXT_BUFFER_SIZE
                jae     @resetTextBufferOffset
                cmp     di, ax
                ja      @addLine

                mov     _textBufferOffset, ax
                jmp     @restoreRegisters

@resetTextBufferOffset:
                mov     _textBufferOffset, 0

@restoreRegisters:
                add     sp, 4 ; ss и cs восстанавливать не нужно
                pop     es
                pop     ds
                popa
                popf
                ret
; конец PrintState

; Ввод:
;     es:di - см. _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     ax, bx, cx, si
_PrintRegisters:
                mov     si, offset _registersTable
                mov     cx, 13

@printRegister:
                mov     bh, _REGISTERS_BACK_COLOR or _LABEL_FORE_COLOR
                lodsw
                call    _PrintWordChar

                mov     bh, _REGISTERS_BACK_COLOR or _VALUE_FORE_COLOR_1
                add     si, 2
                lodsw
                call    _PrintWordHex

                loop    @printRegister

                ret
; конец _PrintRegister

; Ввод:
;     es:di - см. _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     ax, bx, cx
_PrintFlags:
                mov     cx, _flags + 4

                mov     al, 'n'
                rol     cx, 2
                call    _PrintSingleBitFlag

                rol     cx, 2
                mov     ax, cx
                and     ax, 0x0003
                shl     ax, 7
                shr     al, 7
                add     ax, '00'
                mov     bh, _FLAGS_BACK_COLOR or _SET_FLAG_FORE_COLOR
                call    _PrintWordChar

                mov     al, 'o'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 'd'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 'i'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 't'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 's'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 'z'
                rol     cx, 1
                call    _PrintSingleBitFlag

                mov     al, 'a'
                rol     cx, 2
                call    _PrintSingleBitFlag

                mov     al, 'p'
                rol     cx, 2
                call    _PrintSingleBitFlag

                mov     al, 'c'
                rol     cx, 2
                call    _PrintSingleBitFlag

                ret
; конец _PrintFlags

; Ввод:
;     al - код символа, обозначающего данный флаг
;     cx - содержимое флага в младшем бите
;     es:di - см. _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     al, bx
_PrintSingleBitFlag:
                mov     bx, cx
                and     bx, 0x0001
                mov     bh, _flagForeColors[ bx ]
                add     bh, _FLAGS_BACK_COLOR
                call    _PrintByteChar
                ret
; конец _PrintSingleBitFlag

; Ввод:
;     es:di - см. _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     ax, bx, cx, bp
_PrintStack:
                mov     bp, _registersTable_Sp
                mov     cx, _PRINT_STACK_BYTES_COUNT

@printStackWord:
                mov     bx, _PRINT_STACK_BYTES_COUNT
                sub     bx, cx
                and     bx, 0x0007
                jnz     @doNotPrintSeparator

                mov     al, ' '
                mov     bh, _STACK_BACK_COLOR
                call    _PrintByteChar

@doNotPrintSeparator:
                mov     bx, _PRINT_STACK_BYTES_COUNT
                sub     bx, cx
                shr     bx, 1
                and     bx, 0x0001
                mov     bh, _valueForeColors[ bx ]
                or      bh, _STACK_BACK_COLOR
                mov     ax, [ bp ]
                call    _PrintByteHex
                inc     bp
                loop    @printStackWord

                ret
; конец _PrintStack

; Ввод:
;     al - байт, который нужно вывести
;     bh - цвета фона и текста
;     es - сегментный адрес текстового буфера видеокарты (обычно 0xB800)
;     di - позиция в текстовом буфере, в которую нужно произвести вывод
; Вывод:
;     di - позиция в текстовом буфере, следующая за выведенными данными
; Изменяет:
;     al
_PrintByteChar:
                stosb
                mov     al, bh
                stosb
                ret
; конец _PrintByteChar

; Ввод:
;     ax - слово, которое нужно вывести
;     остальное аналогично _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     ax
_PrintWordChar:
                xchg    ah, al
                call    _PrintByteChar
                mov     al, ah
                call    _PrintByteChar
                ret
; конец _PrintWordChar

; Ввод:
;     см. _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     ax
_PrintByteHex:
                call    _ByteToHex
                call    _PrintWordChar
                ret
; конец _PrintByteHex

; Ввод:
;     см. _PrintWordChar
; Вывод:
;     см. _PrintWordChar
; Изменяет:
;     ax
_PrintWordHex:
                push    ax
                mov     al, ah
                call    _PrintByteHex
                pop     ax
                call    _PrintByteHex
                ret
; конец _PrintWordHex

; Ввод:
;     ds:si - адрес выводимой строки, заканчивающейся нулевым символом
;     остальное аналогично _PrintByteChar
; Вывод:
;     см. _PrintByteChar
; Изменяет:
;     al, si
_PrintString:
                lodsb
                test    al, al
                jz      @exitPrintString
                call    _PrintByteChar
                jmp     _PrintString
@exitPrintString:
                ret
; конец _PrintString

; Ввод:
;     al - байт, который нужно сконвертировать в шестнадцатиричное представление
; Вывод:
;     ah - код символа для старшей шестнадцатиричной цифры байта
;     al - код символа для младшей шестнадцатиричной цифры байта
_ByteToHex:
                mov     ah, al

                shr     al, 4
                cmp     al, 0x0A
                sbb     al, 0x69
                das

                xchg    al, ah

                and     al, 0x0F
                cmp     al, 0x0A
                sbb     al, 0x69
                das

                ret
; конец _ByteToHex

                end
