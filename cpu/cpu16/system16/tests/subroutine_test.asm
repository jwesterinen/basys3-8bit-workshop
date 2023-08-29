; subroutine test

; general I/O registers
.define SWITCH_REG          0x2000
.define BUTTON_REG          0x2002
.define LED_REG             0x2010
.define DISPLAY1_REG        0x2020
.define DISPLAY2_REG        0x2021
.define DISPLAY3_REG        0x2022
.define DISPLAY4_REG        0x2023
.define DISPLAY_CTRL_REG    0x2024

; SP = 0x0fff;
; while (1)
; {
;     push(GetSwitch());
;     display(pop());
; }

Begin:
    mov     sp,@$0fff
    
Loop:
    mov     ax,[SWITCH_REG]     ; ax = switch[7:4]
    mov     bx,ax               ; bx = switch[3:0]
    and     ax,#$f0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     [DISPLAY1_REG],ax
    and     bx,#$0f
    mov     [DISPLAY2_REG],bx

    mov     fx,@Add             ; call Add with params in ax and bx
    jsr     fx 
            
EndLoop:
    mov     [DISPLAY4_REG],ax   ; display results
    jmp     Loop

Add:
    add     ax,bx
    rts

