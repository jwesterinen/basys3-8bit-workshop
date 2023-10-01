;   basic_io.asm

;   This program tests the basic I/O of the system16 as follows:
;    - the switches are shown in the LEDs

;   This test the following address modes:
;    - direct store
;    - direct load
;    - direct jump

#include "../system16.h"

.define switchVals  0x123       ; 16-bit RAM address

Begin:
    ; display all zeros
    zero    ax
    mov     DISPLAY1_REG,ax     ; direct load
    mov     DISPLAY2_REG,ax
    mov     DISPLAY3_REG,ax
    mov     DISPLAY4_REG,ax
    
Loop:
    ; show the switch values on the LEDs
    mov     ax,SWITCH_REG           ; direct load
    mov     switchVals,ax           ; direct store to RAM
    mov     bx,switchVals           ; direct load from RAM
    mov     LED_REG,bx 

    jmp     Loop                    ; direct jump

