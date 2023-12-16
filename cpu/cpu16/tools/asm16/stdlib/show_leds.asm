;   show_leds.asm
;
;   Subroutine: _ShowLeds
;
;   Synopsis: void _ShowLeds(int value);
;
;   Args:
;     value: the 16-bit value to show on the LEDs
;
;   Return: none
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          value      (SP+2)
;

#ifndef SHOW_LEDS_ASM
#define SHOW_LEDS_ASM

#define VALUE [bp+3]

_ShowLeds:
    push    bp              ; setup the stack frame
    mov     bp,sp
    
    mov     ax,VALUE        ; show the value on the LEDs
    mov     LED_REG,ax
    
    mov     sp,bp
    pop     bp
    rts    
    
#endif // SHOW_LEDS_ASM
    
