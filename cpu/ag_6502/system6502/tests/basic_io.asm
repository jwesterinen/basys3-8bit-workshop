#include "iodefs.asm"

Begin:
    lda #1              ; set display mode to raw
    sta Display_ctrl
    
Loop:
    lda Switch_LSB      ; store switch values @ 0x0100
    sta $0100
    lda Switch_MSB
    sta $0101
    
    lda Buttons         ; store raw button values @ 0x0102
    sta $0102

    lda $0100           ; load LEDs from 0x0100
    sta LED_LSB
    lda $0101
    sta LED_MSB
    
    lda $0102           ; load all displays from 0x0102
    sta Display1
    sta Display2
    sta Display3
    sta Display4
    
    jmp Loop

