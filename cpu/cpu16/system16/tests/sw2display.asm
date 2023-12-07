;   basic_io.asm
;
;   This program tests the Display() function.  It shows the switch values on the LEDs and display.


#include <asm16/system16.asm>
#include <asm16/sys.asm>
#include <asm16/display.asm>

main:
    mov     ax,SWITCH_REG   ; get the switch value
    mov     LED_REG,ax      ; show the value on the LEDs
    push    ax              ; display the value
    jsr     _Display
    pop     ax 
    jmp     Main

