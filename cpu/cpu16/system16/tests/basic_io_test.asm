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



#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"

.dz switchVals                      ; 8-bit ZP offset
.dz buttonVals                      ; 8-bit ZP offset

Main:
    ; set display mode to raw
    mov     bx,@DISPLAY_CTRL_REG    ; immed16 op
    mov     ax,#1                   ; immed8 op
    mov     [bx],ax                 ; indexed store 0 index
    
Loop:
    ; show the switch values on the LEDs
    mov     bx,@SWITCH_REG
    mov     cx,bx                   ; reg direct op
    mov     ax,[cx]                 ; reg indirect op
    mov     bx,switchVals
    mov     [bx+5],ax               ; indexed store
    mov     cx,[bx+5]               ; indexed load
    mov     bx,@LED_REG           
    mov     [bx],cx 

    ; load all displays from buttons
    mov     bx,@BUTTON_REG
    mov     ax,[bx]
    mov     [#buttonVals],ax        ; ZP store
    mov     bx,@DISPLAY1_REG
    zero    dx
    mov     dx,[#buttonVals]        ; ZP load
    mov     [bx],dx
    inc     bx                      ; reg unary op
    mov     [bx],dx
    inc     bx
    mov     [bx],dx
    inc     bx
    mov     [bx],dx
    
    bra     Loop                    ; IP relative branch

