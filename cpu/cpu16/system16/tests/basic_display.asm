Begin:
    mov     ax,#5           ; write '1' to display 1

Loop:
    ;mov     ax,#0           ; write '0' to display 1
    mov     bx,@0x2020
    mov     [bx],ax
    
    ;mov     ax,#1           ; write '1' to display 1
    mov     bx,@0x2021
    mov     [bx],ax
    
    ;mov     ax,#2           ; write '2' to display 1
    mov     bx,@0x2022
    mov     [bx],ax
    
    ;mov     ax,#3           ; write '3' to display 1
    mov     bx,@0x2023
    mov     [bx],ax
    
    jmp     Loop
    
