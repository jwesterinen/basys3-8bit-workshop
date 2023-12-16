;   read_button.asm
;
;   Subroutine: _ReadButton
;
;   Description: This function returns a button code if a button has been 
;   pressed and released.  If no buttons were pressed 0 is returned. If a button
;   has been pressed a beep is emitted.
;
;   Synopsis: int _ReadButton();
;
;   Args: None
;
;   Return: the button code of the button pressed or 0 if none
;

#ifndef READ_BUTTON_ASM
#define READ_BUTTON_ASM

#include <system16/beep.asm>

_ReadButton:
    push    bx
    
    mov     ax,BUTTON_REG       ; load the button code and return if 0
    or      ax,#0
    bz      _ReadButton_End
    
    push    ax                  ; beep if the app set it up
    jsr     _Beep    
    pop     ax
_ReadButton_L0:
    mov     bx,BUTTON_REG       ; wait for button release
    or      bx,#0
    bnz     _ReadButton_L0
    
_ReadButton_End:         
    pop     bx
    rts    
    
#endif // READ_BUTTON_ASM

