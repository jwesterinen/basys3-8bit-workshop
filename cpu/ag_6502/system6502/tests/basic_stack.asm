#include "iodefs.asm"

adr1 = $200         // 16 bit numbers
adr2 = $202

LoadOp1:            // load 16-bit variables with 0x2345
    lda #$45
    sta adr1        
    lda #$23
    sta adr1+1
    
LoadOp2:            // load 16-bit variables with 0x1234  
    lda #$34
    sta adr2
    lda #$12
    sta adr2+1
    
Begin:
    clc
    cld
    lda adr1        // add LSBs
    ;adc adr2
    sbc adr2
    sta LED_LSB
    lda adr1+1      // add MSBs
    ;adc adr2+1
    sbc adr2+1
    sta LED_MSB

End:
    jmp End
    
