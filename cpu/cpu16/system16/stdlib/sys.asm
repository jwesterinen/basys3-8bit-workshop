;   sys.asm
;
;   This file contains important startup code. It needs to be the first file
;   included in a system16 application, and the beginning of the application
;   must have Main as the starting label.

    mov     sp,@0xfff   ; setup the stack
    jmp     Main        ; jump to the entry point of the application
    
