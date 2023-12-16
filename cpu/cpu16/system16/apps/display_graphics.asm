;   basic_io.asm
;
;   This app allows graphics patterns to be put on the displays as follows:
;    1. Select a character with the switches, 0-0x1c.
;    2. Push the button corresponding to the display:
;        - BTNU for display 1
;        - BTNL for display 2
;        - BTNR for display 3
;        - BTND for display 4


#include <system16/system16.asm>
#include <system16/sys.asm>
#include <system16/libasm.asm>

.dz buttonCode                  ; the code of the last button pressed

main:
    jsr     _ReadButton         ; store the next button pressed
    mov     [#buttonCode],ax
    or      ax,@0
    bz      main
    
DecodeButtons:
    sub     ax,#BUTTON_U        ; decode the buttons to write the switch value
    bz      Display1            ; to the different 7-segment displays
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_L
    bz      Display2
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_R
    bz      Display3
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_D
    bz      Display4
    bra     main

Display1:
    mov     ax,SWITCH_REG
    mov     DISPLAY1_REG,ax
    bra     main

Display2:
    mov     ax,SWITCH_REG
    mov     DISPLAY2_REG,ax
    bra     main

Display3:
    mov     ax,SWITCH_REG
    mov     DISPLAY3_REG,ax
    bra     main

Display4:
    mov     ax,SWITCH_REG
    mov     DISPLAY4_REG,ax
    bra     main


