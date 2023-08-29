.define SWITCH_REG          0x2000
.define BUTTON_REG          0x2002
.define LED_REG             0x2010
.define DISPLAY1_REG        0x2020
.define DISPLAY2_REG        0x2021
.define DISPLAY3_REG        0x2022
.define DISPLAY4_REG        0x2023
.define DISPLAY_CTRL_REG    0x2024

; sound peripheral registers
.define VCO1_REG            0x3000
.define VCO2_REG            0x3001
.define NOISE_REG           0x3002
.define LFO_REG             0x3003
.define LFO_MOD_REG         0x3005
.define MOD_SEL_REG         0x3004  ; {0.., noise, VCO2, VCO1}
.define MIXER_SEL_REG       0x3006  ; {0.., LFO, noise, VCO2, VCO1}

Begin:
    mov ax,#1                   ; set display mode to raw
    mov [DISPLAY_CTRL_REG],ax
    
Loop:
    ;mov ax,[SWITCH_REG]         ; store switch values @ 0x0100
    ;mov [0x100],ax
    
    ;mov ax,[0x0100]             ; load LEDs from 0x0100
    ;mov [LED_REG],ax
    
    mov   bx,@SWITCH_REG
    mov   ax,[bx]
    mov   bx,@LED_REG
    mov   [bx],ax

    mov ax,[BUTTON_REG]         ; store button values @ 0x0101
    mov [0x101],ax

    mov ax,[0x0101]             ; load all displays from 0x0101
    mov bx,@DISPLAY1_REG
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    
    bra Loop

