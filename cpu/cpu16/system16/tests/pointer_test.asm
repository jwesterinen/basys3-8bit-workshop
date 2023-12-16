;   pointer_test.asm

#include <system16/sys.asm>
#include <system16/system16.asm>


main:
; int *pLeds = LED_REG;

    mov     ax,@8208
    push    ax
    
.dw pLeds                       ; pointer to integer
    mov     bx,@pLeds           ; load LedReg with the address of the LED reg
    pop     ax
    mov     [bx],ax

real_main:
; *LedReg = 1234;

    mov     ax,@0x1234
    push    ax

    pop     ax    
    mov     bx,pLeds            ; ledReg contains an address so store indirectly thru it
    mov     [bx],ax
    
End:
    bra     End


