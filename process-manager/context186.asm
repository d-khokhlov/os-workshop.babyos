                .model  tiny
                .186

                public  SaveContext
                public  RestoreContext
                public  InitKernelContext
                public  LoadKernelContext

                .code

; Нет ввода
; Вывод:
;     стек - контекст текущего процесса (содержимое регистров). Регистры ip, cs
;         (адрес точки возобновления процесса) и flags не сохраняются.
;         Функция предназначена для вызова из обработчика прерываний, а значит
;         корректные значения этих ригистров уже находятся в стеке.
;         Также не сохраняется ss и не гарантируется сохранение sp,
;         т.к. не имеет смысла сохранять адрес стека в этом же стеке.
SaveContext:
                pusha
                push    ds
                push    es
                ret
; конец SaveContext

; Ввод:
;     стек - контекст процесса, сохраненный функцией SaveContext.
; Вывод:
;     устанавливаются значения всех регистров (кроме cs, ip, flags, ss и sp,
;     см. описание SaveContext).
RestoreContext:
                pop     es
                pop     ds
                popa
                ret
; конец RestoreContext

; Нет ввода
; Нет вывода
InitKernelContext:
                mov     _kernelSp, sp
                mov     _kernelBp, bp   ; когда будет ассемблерный загрузчик, сохранять и восстанавливать bp будет не нужно
                ret
; конец InitKernelContext

; Нет ввода
; Вывод:
;     восстанавливаются значения ds, es, ss, sp, bp
; Изменяет:
;     ax
LoadKernelContext:
                mov     ax, cs
                mov     ds, ax
                mov     es, ax
                mov     sp, _kernelSp
                mov     bp, _kernelBp
                mov     ss, ax
                ret
; конец RestoreKernelContext

_kernelSp       dw      ?
_kernelBp       dw      ?

                end
