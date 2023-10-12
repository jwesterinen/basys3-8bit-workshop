;   stack_test

;   This program performs arithmetic in a subroutine:
;
;    Display(1, Calc(1));
;    Display(2, Calc(2));
;
;   This test the following address modes:
;    - jsr reg
;    - jsr direct
;    - push
;    - pop
;    - rts

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"

; call a subroutine that will perform RPN calculations. The test passes if "45" is displayed.

Main:
    mov     ex,@Calc
    mov     ax,#1               ; jsr reg
    jsr     ex
    mov     DISPLAY1_REG,ax

    mov     ax,#2        
    jsr     Calc                ; jsr direct
    mov     DISPLAY2_REG,ax
    
End:
    bra     End    
    
Calc:
    push    ax                  ; push instruction
    inc     ax
    push    ax
    inc     ax
    push    ax
    pop     bx                  ; pop instruction
    pop     ax
    add     ax,bx
    pop     bx
    sub     ax,bx
    rts                         ; rts instruction

