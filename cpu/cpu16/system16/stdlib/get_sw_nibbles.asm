;   get_sw_nibbles.asm
;
;   This function returns switch values as 4 nibbles.
;
;   Synopsis: void GetSwNibbles(void);
;
;   Args: none
;
;   Return:
;     ax will contain sw[3:0]
;     bx will contain sw[7:4]
;     cx will contain sw[11:8]
;     dx will contain sw[15:12]
;
;   Regs used:  ax, bx, cx, dx


GetSwNibbles:
    mov     ax,SWITCH_REG       ; ax = switch[3:0]
    mov     bx,ax               ; bx = switch[7:4]
    mov     cx,ax               ; cx = switch[11:8]
    mov     dx,ax               ; dx = switch[15:12]
    
    and     ax,@0x000f

    and     bx,@0x00f0
    mov     ex,#4
GetSwNibbles_L0:    
    lsr     bx
    dec     ex
    bnz     L0
    
    and     cx,@0x0f00
    mov     ex,#8
GetSwNibbles_L1:    
    lsr     cx
    dec     ex
    bnz     L1
    
    and     dx,@0xf000
    mov     ex,#12
GetSwNibbles_L2:    
    lsr     dx
    dec     ex
    bnz     L2
    
    rts
    
    
