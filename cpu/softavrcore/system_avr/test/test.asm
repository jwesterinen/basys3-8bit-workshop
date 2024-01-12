    .org    0x00
    ldi     r16,0x01    ; load 00000001 into register 16
    out     0x04,r16    ; write register 16 to I/O address 4 (LED lsb)
    
    
End:
    jmp     End
    
