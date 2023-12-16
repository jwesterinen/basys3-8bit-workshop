;   read_switches.asm
;
;   Subroutine: _ReadSwitches
;
;   Description: This function returns the value of the switches.
;
;   Synopsis: int _ReadSwitches();
;
;   Args: None
;
;   Return: the button code of the button pressed or 0 if none
;

#ifndef READ_SWITCHES_ASM
#define READ_SWITCHES_ASM

_ReadSwitches:
    mov     ax,SWITCH_REG   ; return the value of the switches
    rts    
    
#endif // READ_SWITCHES_ASM

