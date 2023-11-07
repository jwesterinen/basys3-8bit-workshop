;   beep.asm
;
;   Description:  This file contains beep routines:
;    - _Beep_Init
;    - _Beep
;

#ifndef BEEP_ASM
#define BEEP_ASM

#include "../stdlib/delayms.asm"

.dz _beep_tone                      ; static beep tone
.dz _beep_duration                  ; static beep duration

;   Subroutine: _Beep_Init
;
;   Description: This function initializes the values required by the beep function. 
;
;   Synopsis: void _Beep_Init();
;   Args: None
;   Return: None
;

_Beep_Init:
    mov     ax,@0x080               ; set the default beep tone and duration
    mov     [#_beep_tone],ax        ; and enable VCO1
    mov     ax,@100
    mov     [#_beep_duration],ax
    mov     ax,@MIXER_SEL_VCO1
    mov     MIXER_SEL_REG,ax
    rts

;   Subroutine: _Beep
;
;   Description: This function sets the sound generator period to a pre-defined
;   value for the pre-defined duration then sets the period back to 0.  This
;   causes the sound generator to generate a "beep" typically used as feedback 
;   for key or button presses.

;
;   Synopsis: void _Beep();
;   Args: None
;   Return: None
;

_Beep:
    mov     ax,[#_beep_tone]        ; set to beep tone
    mov     VCO1_REG,ax
    
    mov     ax,[#_beep_duration]    ; turn on VCO1 for duration
    push    ax
    jsr     _DelayMs
    pop     ax
    
    mov     ax,@0                   ; set tone to 0
    mov     VCO1_REG,ax
    
_Beep_End:
    rts
       
#endif // BEEP_ASM
        
