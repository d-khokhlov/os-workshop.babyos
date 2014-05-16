                .model  tiny

                .code

_Start:
                mov     ah, 09h
                mov     dx, offset _message
                int     21h
                retf

_message         db      "Hello, World!", 0Dh, 0Ah, '$'

                end     _Start
