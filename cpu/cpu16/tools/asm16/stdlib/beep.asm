#ifndef BEEP_ASM
#define BEEP_ASM

#include <system16/delayms.asm>

;   Subroutine: _Beep
;
;   Description: Sets the sound generator period to a pre-defined
;   value for the pre-defined duration then sets the period back to 0.  This
;   causes the sound generator to generate a "beep" typically used as feedback 
;   for key or button presses.
;
;   Synopsis: void _Beep();
;
;   Args: None
;
;   Return: None
;

_Beep:
    mov     ax,@MIXER_SEL_VCO1  ; enable VCO1
    mov     MIXER_SEL_REG,ax
    
    mov     ax,@0x080           ; set beep tone to 9kHz
    mov     VCO1_REG,ax
    
    mov     ax,@100             ; delay 100mS
    push    ax
    jsr     _DelayMs
    pop     ax
    
    mov     ax,@0               ; set tone to 0
    mov     VCO1_REG,ax
    
_Beep_End:
    rts
       
#endif // BEEP_ASM
        
