.org 0x0000

    lda #1      ; set display mode to raw
    sta $2024
Loop:
    lda $2000   ; load LEDs from switches
    sta $2010
    lda $2001
    sta $2011
    lda $2002   ; load raw button values to displays
    sta $2020
    sta $2021
    sta $2022
    sta $2023
    jmp Loop

