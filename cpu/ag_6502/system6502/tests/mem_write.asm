.org 0x0000

    ldx #1      ; store 1-4 @ e005-e008
    stx $e005
    inx
    stx $e006
    inx
    stx $e007
    inx
    stx $e008
    
    lda $e005   ; load contents of e005-e008 into displays
    sta $2020
    lda $e006
    sta $2021
    lda $e007
    sta $2022
    lda $e008
    sta $2023
    
Loop:
    jmp Loop

