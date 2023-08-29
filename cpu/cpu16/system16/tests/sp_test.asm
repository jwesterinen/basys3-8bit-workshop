; stack test

; general I/O registers
.define SWITCH_REG          0x2000
.define BUTTON_REG          0x2002
.define LED_REG             0x2010
.define DISPLAY1_REG        0x2020
.define DISPLAY2_REG        0x2021
.define DISPLAY3_REG        0x2022
.define DISPLAY4_REG        0x2023
.define DISPLAY_CTRL_REG    0x2024

; SP = 0xfff;
; while (1)
; {
;     push(GetSwitch());
;     display(pop());
; }

Begin:
    mov     sp,@$0fff
    
Loop:
    mov     ax,[SWITCH_REG]
    push    ax
    zero    ax
    pop     ax
    mov     [LED_REG],ax
    
End:
    jmp     Loop

