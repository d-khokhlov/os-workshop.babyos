                .model  tiny
                .186

                public  DEBUG_PrintState

                .code

_SCREEN_WIDTH \
                equ     80
_SCREEN_HEIGHT  equ     25

_BULLET_COLOR \
                equ     0x58
_FIELD_BACK_COLOR_1 \
                equ     0x10
_FIELD_BACK_COLOR_2 \
                equ     0x20
_FLAGS_BACK_COLOR \
                equ     _FIELD_BACK_COLOR_2
_STACK_BACK_COLOR \
                equ     _FIELD_BACK_COLOR_1n
_LABEL_FORE_COLOR \
                equ     0x0F
_VALUE_FORE_COLOR_1 \
                equ     0x0E
_VALUE_FORE_COLOR_2 \
                equ     0x0D

_textBufferOffset \
                dw      0

_bulletLabel    db      '>>', 0
_stackLabel     db      'stack', 0
_fieldBackColors \
                db      _FIELD_BACK_COLOR_1, _FIELD_BACK_COLOR_2
_valueForeColors \
                db      _VALUE_FORE_COLOR_1, _VALUE_FORE_COLOR_2

_registersTable dw      'ax', ?, 'bx', ?, 'cx', ?, 'dx', ?,
                        'si', ?, 'di', ?, 'bp', ?, 'sp', ?,
                        'ds', ?, 'es', ?, 'ss', ?,
                        'cs', ?, 'ip', ?
_flags          dw      ?


; Нет ввода
; Нет вывода
DEBUG_PrintState:

_stor           macro   register
                mov     [ bx ], register
                add     bx, 4
                endm

                pushf
                pop     _flags

                push    bx
                mov     bx, offset _registersTable + 2

                _stor   ax
                pop     ax
                _stor   ax
                _stor   cx
                _stor   dx
                _stor   si
                _stor   di
                _stor   bp
                _stor   /sp
                add     word ptr [ bx - 4 ], 2
                _stor   ds
                _stor   es
                _stor   ss
                _stor   cs
                _stor   /ip
                _stor
                _stor
                _stor
                _stor
                _stor

                push    ax
                push    bx
                push    si
                push    di
                push    es
                push    cx
                push    dx

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

                mov     bl, _BULLET_COLOR
                mov     si, offset _bulletLabel
                call    _PrintString

                call    _PrintRegisters

                call    _PrintFlags

                call    _PrintStack

                mov     _textBufferOffset, di

                pop     dx
                pop     cx
                pop     es
                pop     di
                pop     si
                pop     bx
                pop     ax

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
                mov     bl, 0

@printRegister:
                and     bx, 0x0001
                mov     bh, _fieldBackColors[ bx ]

                add     bh, _LABEL_FORE_COLOR
                lodsw
                call    _PrintWordChar

                and     bh, 0xF0
                add     bh, _VALUE_FORE_COLOR_1
                lodsw
                call    _PrintWordHex

                inc     bl

                loop    @printRegister

                ret
; конец _PrintRegister

;
_PrintFlags:
                ret
; конец _PrintFlags

;
_PrintStack:
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
