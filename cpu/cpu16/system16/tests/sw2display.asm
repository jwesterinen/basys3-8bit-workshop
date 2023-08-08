Loop:
    mov     bx,@0x2000  ; display 4 = (sw & 0x000f)
    mov     ax,[bx]
    and     ax,@0x000f
    mov     bx,@0x2023
    mov     [bx],ax

    mov     bx,@0x2000  ; display 3 = (sw & 0x00f0) >> 4
    mov     ax,[bx]
    and     ax,@0x00f0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     bx,@0x2022
    mov     [bx],ax

    mov     bx,@0x2000  ; display 2 = (sw & 0x0f00) >> 8
    mov     ax,[bx]
    and     ax,@0x0f00
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     bx,@0x2021
    mov     [bx],ax
    
    mov     bx,@0x2000  ; display 1 = (sw & 0xf000) >> 12
    mov     ax,[bx]
    and     ax,@0xf000
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     bx,@0x2020
    mov     [bx],ax

    jmp     Loop

