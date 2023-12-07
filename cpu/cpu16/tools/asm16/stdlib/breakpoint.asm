    
Breakpoint:
    mov     DISPLAY1_REG,ax
    mov     DISPLAY2_REG,ep
    mov     DISPLAY3_REG,cx
    mov     DISPLAY4_REG,dx
Breakpoint_loop:
    bra     Breakpoint_loop    


