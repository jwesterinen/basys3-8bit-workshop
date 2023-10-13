;   get_sw_bytes.asm
;
;   This function returns switch values as 2 bytes.
;
;   Synopsis: void GetSwBytes(void);
;
;   Args: none
;
;   Return:
;     ax will contain sw[7:0]
;     bx will contain sw[15:8]
;
;   Regs used:  ax, bx


GetSwBytes:
    mov     ax,SWITCH_REG       ; ax = switch[7:0]
    mov     bx,ax               ; bx = switch[15:8]
    
    and     ax,@0x00ff

    and     bx,@0xff00
    mov     ex,#8
GetSwBytes_L0:    
    lsr     bx
    dec     ex
    bnz     L0
    
    rts
    
    
