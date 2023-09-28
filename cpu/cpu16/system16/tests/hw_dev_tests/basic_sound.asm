.org 0xf000

Loop:
    ; VCO1
    mov     bx,@0x2000  ; VCO1 = (sw & 0x0007) << 6
    mov     ax,[bx]
    and     ax,@0x0007
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     bx,@0x3000
    mov     [bx],ax

    ; VCO2
    mov     bx,@0x2000  ; VCO2 = ((sw & 0x0038) >> 3) << 6
    mov     ax,[bx]
    and     ax,@0x0038
    asl     ax
    asl     ax
    asl     ax
    mov     bx,@0x3001
    mov     [bx],ax

    ; mixer
    mov     ax,@0x0003  ; mix in VCO1, VCO2
    mov     bx,@0x3006
    mov     [bx],ax
    
    jmp     Loop

