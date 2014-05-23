                .model  tiny
                .186

                public  SaveContext
                public  RestoreContext
                public  C SaveNewContext
                public  C InitKernelContext
                public  LoadKernelContext

                extern  C AllocateFarMemory : near
                extern  DEBUG_PrintState : near
                extern  DEBUG_Pause : near

                .code

_KERNEL_STACK_SIZE \
                equ     2048

_kernelSs       dw      ?
_kernelSp       dw      ?

_returnOffset   dw      ?

; Нет ввода
; Вывод:
;     стек - контекст текущего процесса (содержимое регистров). Регистры ip, cs
;         (адрес точки возобновления процесса) и flags не сохраняются.
;         Функция предназначена для вызова из обработчика прерываний, а значит
;         корректные значения этих ригистров уже находятся в стеке.
;         Также не сохраняется ss и не гарантируется сохранение sp,
;         т.к. не имеет смысла сохранять адрес стека в этом же стеке.
SaveContext:
                call    DEBUG_PrintState
                call    DEBUG_Pause

                pop     [ cs:_returnOffset ]
                pusha
                push    ds
                push    es
                jmp     [ cs:_returnOffset ]
; конец SaveContext

; Ввод:
;     стек - контекст процесса, сохраненный функцией SaveContext.
; Вывод:
;     устанавливаются значения всех регистров (кроме cs, ip, flags, ss и sp,
;     см. описание SaveContext).
RestoreContext:
                pop     [ cs:_returnOffset ]
                pop     es
                pop     ds
                popa
                jmp     [ cs:_returnOffset ]
; конец RestoreContext

; CommonRegister _cdecl SaveNewContext(
;     SegmentRegister cs, CommonRegister ip,
;     SegmentRegister ss, CommonRegister sp,
;     SegmentRegister ds, SegmentRegister es );
SaveNewContext:
                push    bp
                mov     bp, sp
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                push    ds
                push    es

                mov     bx, [ bp + 4 ]
                mov     si, [ bp + 6 ]
                mov     es, [ bp + 12 ]
                mov     ds, [ bp + 14 ]
                mov     ax, [ bp + 16 ]

                mov     di, sp
                mov     cx, ss

                mov     dx, [ bp + 10 ]
                and     dx, 0xFFFE
                mov     ss, [ bp + 8 ]
                mov     sp, dx

                push    ax
                pushf
                push    bx
                push    si
                pusha
                push    es
                push    ds

                mov     ax, sp

                mov     ss, cx
                mov     sp, di

                pop     es
                pop     ds
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
; конец SaveNewContext

; Нет ввода
; Нет вывода
InitKernelContext:
                push    word ptr 0
                push    word ptr _KERNEL_STACK_SIZE
                call    AllocateFarMemory
                add     sp, 4
                mov     _kernelSs, ax

                mov     ax, _KERNEL_STACK_SIZE
                and     ax, 0xFFFE
                mov     _kernelSp, ax

                ret
; конец InitKernelContext

; Нет ввода
; Вывод:
;     восстанавливаются значения ds, es, ss, sp
; Изменяет:
;     ax
LoadKernelContext:
                pop     [ cs:_returnOffset ]
                mov     ax, cs
                mov     ds, ax
                mov     es, ax
                mov     ss, _kernelSs
                mov     sp, _kernelSp
                jmp     [ cs:_returnOffset ]
; конец RestoreKernelContext

                end
