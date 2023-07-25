.org 0x0000

Start:
    lda #1      ; set raw display
    sta $2024
Loop:    
    lda $2002   ; display button values on all displays
    sta $2020
    sta $2021
    sta $2022
    sta $2023
    jmp Loop

