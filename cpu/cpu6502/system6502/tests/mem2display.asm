.org 0x0000

    lda #0      ; store 1-4 @ e005-e008
    sta $e005
    lda #1 
    sta $e006
    lda #2
    sta $e007
    lda #3
    sta $e008
    
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

