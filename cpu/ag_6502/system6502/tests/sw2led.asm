#include "iodefs.asm"

Loop:
    lda Switch_LSB
    sta LED_LSB
    lda Switch_MSB
    sta LED_MSB
    jmp Loop

