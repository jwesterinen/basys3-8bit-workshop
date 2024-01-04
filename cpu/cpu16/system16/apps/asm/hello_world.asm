;   hello_world
;
;   This is the canonical HW hello world HW app.  It blinks the LEDs once a second.

#include <system16/system16.asm>
#include <system16/sys.asm>
#include <system16/delayms.asm>

main:
    mov     ax,@0xffff      ; init LED value

Loop:    
    mov     LED_REG,ax      ; set the current LED value

    mov     ax,@1000        ; call DelayMs(500)
    push    ax
    jsr     _DelayMs
    pop     ax              ; (clean up the stack)
    
DelayMs_toggle_leds:
    mov     ax,LED_REG      ; retrieve and toggle the LED value
    xor     ax,@0xffff
    bra     Loop
    

