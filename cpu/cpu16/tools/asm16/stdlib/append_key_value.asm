;   append_key_value.asm
;
;   Subroutine: _AppendKeyValue
;
;   Description: This function checks for a new key press and modifies the value passed
;   in by shifting it left 4 then appending the new hextet from the keypad.
;   If a newEntry is specified, the value is cleared before appending
;   the new hextet thus implementing a "new entry" starting from 0x0000.
;
;   Synopsis: int _AppendKeyValue(int value, int newEntry);
;
;   Args:
;     value:    the 16-bit value to modify
;     newEntry: the "new entry" flag, non-zero for new entry
;
;   Return: the modified value
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          newEntry   (SP+2)
;          value      (SP+3)
;

#ifndef APPEND_KEY_VALUE_ASM
#define APPEND_KEY_VALUE_ASM

#include <system16/beep.asm>

#define NEW_ENTRY   [bp+3]
#define VALUE       [bp+4]

_AppendKeyValue:
    push    bp              ; setup the stack frame
    mov     bp,sp
    push    bx
    push    cx

    jsr     _ReadKeypad     ; check for a key
    mov     bx,ax           ; bx has keycode
    mov     ax,VALUE        ; ax has value
    
    or      bx,#0           ; just return the original value if no keypress
    bz      _AppendKeyValue_End

    mov     cx,NEW_ENTRY
    or      cx,#0           ; if the new entry flag is set clear the value
    bz      _AppendKeyValue_L1
    zero    ax

_AppendKeyValue_L1:              
    asl     ax              ; value = (value << 4) | (keycode & 0x000f)
    asl     ax
    asl     ax
    asl     ax    
    and     bx,@0x000f      ; convert key code to key value
    or      ax,bx

_AppendKeyValue_End:
    pop     cx
    pop     bx
    mov     sp,bp
    pop     bp
    rts

#endif // APPEND_KEY_VALUE_ASM

