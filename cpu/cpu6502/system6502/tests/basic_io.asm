.org 0xe000

    lda #1      ; set display mode to raw
    sta $2024
    
Loop:
    lda $2000   ; store switch values @ 0x0100
    sta $0100
    lda $2001
    sta $0101
    
    lda $2002   ; store raw button values @ 0x0102
    sta $0102

    lda $0100   ; load LEDs from 0x0100
    sta $2010
    lda $0101
    sta $2011
    
    lda $0102   ; load all displays from 0x0102
    sta $2020
    sta $2021
    sta $2022
    sta $2023
    
    jmp Loop

