;   display.asm
;
;   This function displays a 16-bit value in hex.
;
;   Synopsis: void Display(int value);
;
;   Args:
;     value: the 16-bit value to display
;   Return: none
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          value      (SP+2)
;
;   Regs used:  ax, bx
;

;------------------------------------------------------------------------------
; test:
;
;#include "system16.asm"
;#include "sys.asm"
;;#include "display.asm"    ; this must exist in an app
;
;Main:
;    mov     ax,@0xabcd
;    push    ax
;    jsr     Display
;    pop     ax
;End:
;    bra     End
;------------------------------------------------------------------------------

Display:
    mov     bp,sp           ; init the stack frame
    
    mov     ax,[bp+2]       ; display high nibble in display1
    and     ax,@0xf000
    mov     bx,#12
Display_L1:
    lsr     ax
    dec     bx
    bnz     Display_L1
    mov     DISPLAY1_REG,ax
    
    mov     ax,[bp+2]       ; display next to highest nibble in display2
    and     ax,@0x0f00
    mov     bx,#8
Display_L2:
    lsr     ax
    dec     bx
    bnz     Display_L2
    mov     DISPLAY2_REG,ax
    
    mov     ax,[bp+2]       ; display next to lowest nibble in display3
    and     ax,@0x00f0
    mov     bx,#4
Display_L3:
    lsr     ax
    dec     bx
    bnz     Display_L3
    mov     DISPLAY3_REG,ax
    
    mov     ax,[bp+2]       ; display lowest nibble in display4
    and     ax,@0x000f
    mov     DISPLAY4_REG,ax
    
Display_end:
    rts    
    
