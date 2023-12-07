;   input.asm
;
;   Description:  This file contains system16 input device subroutines:
;    - _ReadKeypad
;    - _ReadButton
;    - _AppendKeyValue
;    - _GetSwBytes
;    - _GetSwNibbles


;   Subroutine: _ReadKeypad
;
;   Description: This function returns the keycode if a key has been 
;   pressed and released.  If no keys were pressed 0 is returned.
;
;   Synopsis: void _ReadKeypad();
;
;   Args: None
;
;   Return: the keycode in ax

#ifndef INPUT_ASM
#define INPUT_ASM

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

    
;   Subroutine: _ReadButton
;
;   Description: This function returns the value of a button if it has been pressed 
;   and released.  If no buttons were pressed 0 is returned
;
;   Synopsis: void _ReadButton();
;
;   Args: None
;
;   Return: the button code in ax
;

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
    

;   Subroutine: _AppendKeyValue
;
;   Description: This function checks for a new key press and modifies the value passed
;   in ax by shifting it left 4 then appending the new hextet from the keypad.
;   If the dx reg contains a non-zero value, the value is cleared before appending
;   the new hextet thus implementing a "new entry" starting from 0x0000.
;
;   Synopsis: void _AppendKeyValue();
;
;   Args:
;     ax: the 16-bit value to modify in ax
;     dx: the "new entry" flag, non-zero for new entry
;
;   Return: the modified value in ax
;

_AppendKeyValue:
    push    bx
    
    push    ax              ; get the keycode if a key is pressed
    jsr     _ReadKeypad
    mov     bx,ax           ; bx has keycode
    pop     ax              ; ax has value
    
    or      bx,#0           ; just return the original value if no keypress
    bz      _AppendKeyValue_End

    or      dx,#0           ; if the new entry flag is set, clear the value and flag
    bz      _AppendKeyValue_L1
    zero    ax
    mov     dx,#0

_AppendKeyValue_L1:              
    asl     ax              ; value = (value << 4) | (keycode & 0x000f)
    asl     ax
    asl     ax
    asl     ax    
    and     bx,@0x000f      ; convert key code to key value
    or      ax,bx

_AppendKeyValue_End:
    pop     bx
    rts


;   get_sw_bytes.asm
;
;   This function returns switch values as 2 bytes.
;
;   Synopsis: void _GetSwBytes(void);
;
;   Args: none
;
;   Return:
;     ax will contain sw[7:0]
;     bx will contain sw[15:8]


_GetSwBytes:
    push    cx
    
    mov     ax,SWITCH_REG       ; ax = switch[7:0]
    mov     bx,ax               ; bx = switch[15:8]    
    and     ax,@0x00ff
    and     bx,@0xff00
    mov     cx,#8
_GetSwBytes_L0:    
    lsr     bx
    dec     cx
    bnz     _GetSwBytes_L0

    pop     cx    
    rts
    
    
;   get_sw_nibbles.asm
;
;   This function returns switch values as 4 nibbles.
;
;   Synopsis: void _GetSwNibbles(void);
;
;   Args: none
;
;   Return:
;     ax will contain sw[3:0]
;     bx will contain sw[7:4]
;     cx will contain sw[11:8]
;     dx will contain sw[15:12]


_GetSwNibbles:
    mov     ax,SWITCH_REG       ; ax = switch[3:0]
    mov     bx,ax               ; bx = switch[7:4]
    mov     cx,ax               ; cx = switch[11:8]
    mov     dx,ax               ; dx = switch[15:12]
    
    and     ax,@0x000f
    push    ax

    and     bx,@0x00f0
    mov     ax,#4
_GetSwNibbles_L0:    
    lsr     bx
    dec     ax
    bnz     _GetSwNibbles_L0
    
    and     cx,@0x0f00
    mov     ax,#8
_GetSwNibbles_L1:    
    lsr     cx
    dec     ax
    bnz     _GetSwNibbles_L1
    
    and     dx,@0xf000
    mov     ax,#12
_GetSwNibbles_L2:    
    lsr     dx
    dec     ax
    bnz     _GetSwNibbles_L2

    pop     ax    
    rts

#endif // INPUT_ASM

