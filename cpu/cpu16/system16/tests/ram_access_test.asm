;   branch_test.asm
; 
;   Description: This program tests RAM access via reg-indir addressing.
;
;   Write each RAM location to itself from 0xfff0 to 0, then read all
;   values back from 0x0fff to 0.  If there are any read values that are
;   different than their address, show the address on the LEDs, else show 
;   0xffff to indicate the test passed.


#include "../stdlib/system16.asm"

Begin:
    mov     ax,@0
    mov     bx,@0x1000
    
WriteLoop:
    mov     [ax],ax
    inc     ax
    dec     bx
    bnz     WriteLoop    

    mov     ax,@0
    mov     bx,@0x1000
ReadLoop:    
    mov     cx,[ax]
    sub     cx,ax
    bnz     End
    inc     ax
    dec     bx
    bnz     ReadLoop
    mov     ax,@0xffff
    
End:
    mov     LED_REG,ax
    
