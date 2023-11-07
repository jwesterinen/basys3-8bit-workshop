;   sys.asm
;
;   This file contains important startup code. It needs to be the first file
;   included in a system16 application, and the beginning of the application
;   must have Main as the starting label.

    mov     sp,@0x0fff  ; init the code stack
    mov     ep,@0x0dff  ; init the evaluation stack
    zero    ep          ; init the expression stack
    
    jmp     Main        ; jump to the entry point of the application

