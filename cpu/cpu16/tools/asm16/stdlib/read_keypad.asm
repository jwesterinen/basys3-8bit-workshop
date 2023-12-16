;   read_keypad.asm
;
;   Subroutine: _ReadKeypad
;
;   Description: This function returns a keycode if a key has been 
;   pressed and released.  If no keys were pressed 0 is returned. If a key
;   has been pressed a beep is emitted.
;
;   Synopsis: int _ReadKeypad();
;
;   Args: None
;
;   Return: the keycode of the pressed key or 0 if none
;

#ifndef READ_KEYPAD_ASM
#define READ_KEYPAD_ASM

#include <system16/beep.asm>

_ReadKeypad:
    push    bx
    
    mov     ax,KEYPAD_REG       ; load the button code and return if 0
    or      ax,#0
    bz      _ReadKeypad_End

    push    ax                  ; beep if the app set it up
    jsr     _Beep    
    pop     ax
_ReadKeypad_L0:
    mov     bx,KEYPAD_REG       ; wait for button release
    or      bx,#0
    bnz     _ReadKeypad_L0
    
_ReadKeypad_End:         
    pop     bx
    rts    

#endif // READ_KEYPAD_ASM

