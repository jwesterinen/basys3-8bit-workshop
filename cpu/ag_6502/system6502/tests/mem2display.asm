.org 0xe000

Begin:
    lda #0      ; store 1-4 @ 0005-0008
    sta $0005
    lda #1 
    sta $0006
    lda #2
    sta $0007
    lda #3
    sta $0008
    
    lda $0005   ; load contents of 0005-0008 into displays
    sta $2020
    lda $0006
    sta $2021
    lda $0007
    sta $2022
    lda $0008
    sta $2023
    
Loop:
    jmp Loop

