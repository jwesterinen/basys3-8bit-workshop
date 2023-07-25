#include "iodefs.asm"

op1 = $200
op2 = $202

LoadOps:
    lda #$2
    sta op1
    lda #$1
    sta op2
    
Begin:
;    clc
    lda op1
    adc op2
;    sbc op2
    sta LED_LSB

End:
    jmp End
    
