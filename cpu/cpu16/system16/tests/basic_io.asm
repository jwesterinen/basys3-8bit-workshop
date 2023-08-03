Begin:
    mov ax,@1       ; set display mode to raw
    mov bx,@0x2024
    mov [bx],ax
    
Loop:
    mov bx,@0x2000  ; store switch values @ 0x0100
    mov ax,[bx]
    mov bx,@0x0100
    mov [bx],ax
    
    mov bx,@0x2002  ; store raw button values @ 0x0101
    mov ax,[bx]
    mov bx,@0x0101
    mov [bx],ax

    mov bx,@0x0100  ; load LEDs from 0x0100
    mov ax,[bx]
    mov bx,@0x2010
    mov [bx],ax
    
    mov bx,@0x0101  ; load all displays from 0x0101
    mov ax,[bx]
    mov bx,@0x2020
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    
    jmp Loop

