;   basic_io.asm

;   This program tests the basic I/O of the system16 as follows:
;    - the switches are shown in the LEDs
;    - the buttons are shown in the display segments

;   This test the following address modes:
;    - immed16 op
;    - immed8 op
;    - indexed store 0 index
;    - reg direct op
;    - reg indirect op    
;    - indexed store
;    - indexed load
;    - ZP store
;    - ZP load
;    - reg unary op
;    - branch



#include "../system16.h"

.define switchVals  0x10        ; 8-bit ZP offset
.define buttonVals  0x11        ; 8-bit ZP offset

Begin:
    ; set display mode to raw
    mov bx,@DISPLAY_CTRL_REG    ; immed16 op
    mov ax,#1                   ; immed8 op
    mov [bx],ax                 ; indexed store 0 index
    
Loop:
    ; show the switch values on the LEDs
    mov bx,@SWITCH_REG
    mov cx,bx                   ; reg direct op
    mov ax,[cx]                 ; reg indirect op
    mov bx,switchVals
    mov [bx+5],ax               ; indexed store
    mov cx,[bx+5]               ; indexed load
    mov bx,@LED_REG           
    mov [bx],cx 

    ; load all displays from buttons
    mov bx,@BUTTON_REG
    mov ax,[bx]
    mov [#buttonVals],ax        ; ZP store
    mov bx,@DISPLAY1_REG
    mov cx,[#buttonVals]        ; ZP load
    mov [bx],cx
    inc bx                      ; reg unary op
    mov [bx],cx
    inc bx
    mov [bx],cx
    inc bx
    mov [bx],cx
    
    bra Loop                    ; IP relative branch

