                .model  tiny

                .code

start:
                mov     ah, 09h
                mov     dx, offset message
                int     21h
                retf

message         db      "Hello, World!", 0Dh, 0Ah, '$'

                end     start
