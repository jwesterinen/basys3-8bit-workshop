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

.define value               0x0000

; SP = 0x0fff;
; while (1)
; {
;     ax = GetSwitch[7:4];
;     bx = GetSwitch[3:0];
;     value = bx;
;     ax += value;
; }

Loop:
    mov     ax,[SWITCH_REG]     ; ax = switch[7:4]
    mov     bx,ax               ; bx = switch[3:0]
    and     ax,#$f0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     [DISPLAY1_REG],ax   ; display ax
    and     bx,#$0f
    mov     [DISPLAY2_REG],bx   ; display bx

    mov     [value],bx
    and     ax,[value] 
    mov     [DISPLAY4_REG],ax   ; display results
            
    jmp     Loop

