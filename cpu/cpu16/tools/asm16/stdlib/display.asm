;   display.asm
;
;   Subroutine: _Display
;
;   Description: This subroutine displays a 16-bit value in hex on the four 7-segment displays
;
;   Synopsis: void _Display(int value);
;
;   Args:
;     value: the 16-bit value to display
;
;   Return: none
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          value      (SP+2)
;

#ifndef DISPLAY_ASM
#define DISPLAY_ASM

#define VALUE [bp+3]

_Display:
    push    bp              ; setup the stack frame
    mov     bp,sp
    push    bx
    
    mov     ax,VALUE        ; display high nibble in display1
    and     ax,@0xf000
    mov     bx,#12
_Display_L1:
    lsr     ax
    dec     bx
    bnz     _Display_L1
    mov     DISPLAY1_REG,ax
    
    mov     ax,VALUE        ; display next to highest nibble in display2
    and     ax,@0x0f00
    mov     bx,#8
_Display_L2:
    lsr     ax
    dec     bx
    bnz     _Display_L2
    mov     DISPLAY2_REG,ax
    
    mov     ax,VALUE        ; display next to lowest nibble in display3
    and     ax,@0x00f0
    mov     bx,#4
_Display_L3:
    lsr     ax
    dec     bx
    bnz     _Display_L3
    mov     DISPLAY3_REG,ax
    
    mov     ax,VALUE        ; display lowest nibble in display4
    and     ax,@0x000f
    mov     DISPLAY4_REG,ax
    
_Display_end:
    pop     bx              ; restore stack and return
    mov     sp,bp
    pop     bp
    rts    
    
#endif // DISPLAY_ASM
    
