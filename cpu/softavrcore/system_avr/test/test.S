    rjmp    start
    
    ldi     r16,0xff    ; load 0xff into register 16
    out     0x04,r16    ; write register 16 to I/O address 4 (LED lsb)
    
start:
    ldi     r16,0x12    ; load 0x01 into register 16
    out     0x04,r16    ; write register 16 to I/O address 4 (LED lsb)
    
end:
    rjmp    end
        

