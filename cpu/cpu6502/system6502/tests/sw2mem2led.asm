.org 0x0000

Start:
    lda $2000
    sta $e005
    lda #0
    lda $e005
    sta $2001
    jmp Start

