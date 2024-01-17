main:
    rjmp    begin
    nop
    nop
    
begin:    
    in      r16,0x00    ; write switches to LEDs
    out     0x04,r16
    in      r16,0x01
    out     0x05,r16

    in      r16,0x03    ; write (keycode & 0x000f) to display 0
    ldi     r17,0x0f
    and     r16,r17
    out     0x0c,r16
    
    in      r16,0x02    ; write buttons to displays 1-3
    out     0x0d,r16
    out     0x0e,r16
    out     0x0f,r16
    
end:
    rjmp    begin
        

