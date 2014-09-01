; worm.asm
; Игра "Питон" (или "Змея", или "Червяк"). Управление осуществляется клавишами
; управления курсором, питон погибает, если он выходит за верхнюю или нижнюю
; границу экрана или самопересекается.
;
; bug: Иногда после загрузки (возможно только до того момента, как питон
;     вырастет до начальной длины) при нажатии клавиши вверх останавливается вся
;     система (включая этот и другие процессы).
;
; bug: Если повернуть питона на 180 градусов (например движемся влево и нажимаем
;     клавишу вправо), весь экран заполняется каким-то символом (один из
;     символов рисования таблиц серым цветом на красном фоне).

                .model tiny
                .code
                .186

POINT_SIZE      equ     2
SCREEN_WIDTH    equ     80
SCREEN_HEIGHT   equ     15

MOVE_UP         equ     -SCREEN_WIDTH * POINT_SIZE
MOVE_DOWN       equ     SCREEN_WIDTH * POINT_SIZE
MOVE_LEFT       equ     -POINT_SIZE
MOVE_RIGHT      equ     POINT_SIZE

FOOD_POINT      equ     '$' + 0x0A00
BODY_POINT      equ     '*' + 0x0F00
HEAD_POINT      equ     '@' + 0x0E00
WALL_POINT      equ     0x06DB
EMPTY_POINT     equ     ' ' + 0x0000

START_BODY_SIZE equ     4
MAX_BODY_SIZE   equ     128
BODY_INDEX_MASK equ     MAX_BODY_SIZE - 1
BODY_GROWTH_ON_EAT \
                equ     2

_Start:
                mov     ah, 0
                int     0x1A
                mov     _seed, dx

                push    0xB800
                pop     es

                call    _InitScreen
                call    _CreateFood

                call    _RandomEmptyOffset
                mov     _body[ 0 ], di

                mov     bp, START_BODY_SIZE - 1

@mainCycle:
                mov     dx, 0x0500
@delayLoopOut:
                mov     cx, 0xFFFF
@delayLoopIn:
                loop    @delayLoopIn
                dec     dx
                jnz     @delayLoopOut
                ;mov     dx, 0
                ;mov     cx, 2
                ;mov     ah, 0x86
                ;int     0x15

                ; hack: Похоже, обработчик прерывания 0x16 как-то конфликтует с
                ; с ядром, когда оно обрабатывает прерывание от таймера во время
                ; работы этого обработчика. Поэтому запрещаем прерывание от
                ; таймера на время вызова прерывания 0x16.
                in      al, 0x21
                or      al, 00000001b
                out     0x21, al

                mov     ah, 1
                int     0x16

                ; hack: см. выше
                pushf
                in      al, 0x21
                and     al, 11111110b
                out     0x21, al
                popf

                jz      @checkBodyGrowing

                ; hack: см. выше
                in      al, 0x21
                or      al, 00000001b
                out     0x21, al

                mov     ah, 0
                int     0x16

                ; hack: см. выше
                in      al, 0x21
                and     al, 11111110b
                out     0x21, al

@checkUpPressed:
                cmp     ah, 0x48
                jne     @checkDownPressed
                mov     word ptr _moveDirection, MOVE_UP

@checkDownPressed:
                cmp     ah, 0x50
                jne     @checkLeftPressed
                mov     word ptr _moveDirection, MOVE_DOWN

@checkLeftPressed:
                cmp     ah, 0x4B
                jne     @checkRightPressed
                mov     word ptr _moveDirection, MOVE_LEFT

@checkRightPressed:
                cmp     ah, 0x4D
                jne     @checkBodyGrowing
                mov     word ptr _moveDirection, MOVE_RIGHT

@checkBodyGrowing:
                cmp     bp, 0
                je      @eraseTail
                dec     bp
                jmp     @advanceHead

@eraseTail:
                mov     bx, _tailIndex
                call    _GetBodyElement
                mov     word ptr es:di, EMPTY_POINT
                call    _GetNextBodyIndex
                mov     _tailIndex, bx

@advanceHead:
                mov     bx, _headIndex
                call    _GetBodyElement
                mov     word ptr es:di, BODY_POINT

                call    _GetNextBodyIndex
                mov     _headIndex, bx

                add     di, _moveDirection
                call    _SetBodyElement

                mov     ax, es:di
                mov     word ptr es:di, HEAD_POINT

                cmp     ax, EMPTY_POINT
                je      @endMainCycle

                cmp     ax, FOOD_POINT
                je      @eatFood

                retf

@eatFood:
                call    _CreateFood
                mov     bp, BODY_GROWTH_ON_EAT

@endMainCycle:
                jmp     @mainCycle

; Ввод:
;     es - сегмент видеопамяти
; Нет вывода
; Изменяет:
;     ax, cx, di
_InitScreen:
                cld

                mov     ax, WALL_POINT
                mov     cx, SCREEN_WIDTH
                mov     di, 0
                rep stosw

                mov     ax, EMPTY_POINT
                mov     cx, SCREEN_WIDTH * ( SCREEN_HEIGHT - 2 )
                rep stosw

                mov     ax, WALL_POINT
                mov     cx, SCREEN_WIDTH
                rep stosw

                mov     cx, SCREEN_HEIGHT - 2
                mov     di, SCREEN_WIDTH * POINT_SIZE

@drawVerticalWalls:
                stosw
                add     di, ( SCREEN_WIDTH - 2 ) * POINT_SIZE
                stosw
                loop    @drawVerticalWalls

                ret

; Ввод:
;     es - сегмент видеопамяти
; Нет вывода
; Изменяет:
;     ax, dx, di
_CreateFood:
                call    _RandomEmptyOffset
                mov     word ptr es:di, FOOD_POINT
                ret

; Ввод:
;     bx - исходный индекс в массиве тела
; Вывод:
;     bx - следующий индекс
_GetNextBodyIndex:
                inc     bx
                and     bx, BODY_INDEX_MASK
                ret

; Ввод:
;     bx - индекс в массиве тела
; Вывод:
;     di - элемент тела с заданным индексом
; Изменяет:
;     si
_GetBodyElement:
                mov     si, bx
                shl     si, 1
                add     si, offset _body
                mov     di, [ si ]
                ret

; Ввод:
;     bx - индекс в массиве тела
;     di - новый элемент тела для данного индекса
; Нет вывода
; Изменяет:
;     si
_SetBodyElement:
                mov     si, bx
                shl     si, 1
                add     si, offset _body
                mov     [ si ], di
                ret

; Ввод:
;     es - сегмент видеопамяти
; Вывод:
;     di - смещение случайной пустой точки на экране
; Изменяет:
;     ax, dx
_RandomEmptyOffset:
                call    _Random
                cmp     ax, SCREEN_WIDTH * SCREEN_HEIGHT * POINT_SIZE
                jae     _RandomEmptyOffset

                mov     di, ax
                mov     ax, es:di
                cmp     ax, EMPTY_POINT
                jne     _RandomEmptyOffset

                ret

; Нет ввода
; Вывод:
;     ax - случайное число
; Изменяет:
;     dx
_Random:
                mov     ax, _seed
                mov     dx, 8E45h
                mul     dx
                inc     ax
                mov     _seed, ax
                ret

_moveDirection  dw      MOVE_RIGHT
_headIndex      dw      0
_tailIndex      dw      0
_seed           dw      ?
_body           dw      MAX_BODY_SIZE dup( ? )

                end     _Start
