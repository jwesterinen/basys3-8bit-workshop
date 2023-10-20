;   basic_io.asm

;   This program tests the basic I/O of the system16 as follows:
;    - the raw keypad codes are shown in the LEDs
;    - the translated values are shown in display 4

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"

Main:
    mov     ax,KEYPAD_REG
    mov     LED_REG,ax
    or      ax,@0
    bnz     DecodeKeyValue
    
BlankDisplay:
    mov     ax,@0x0010
    bra     DisplayValue

DecodeKeyValue:
    and     ax,@0x000f
    
DisplayValue:    
    mov     DISPLAY4_REG,ax
    bra     Main

