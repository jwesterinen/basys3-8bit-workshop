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
    mov ax,#1       ; set display mode to raw
;    mov bx,@0x2024
;    mov [bx],ax
    mov [DISPLAY_CTRL_REG],ax
    
Loop:
;    mov bx,@0x2000     ; store switch values @ 0x0100
;    mov ax,[bx]
;    mov bx,@0x0100
;    mov [bx],ax
    mov ax,[SWITCH_REG] ; store switch values @ 0x0100
    mov [0x100],ax
    
;    mov bx,@0x2002     ; store raw button values @ 0x0101
;    mov ax,[bx]
;    mov bx,@0x0101
;    mov [bx],ax
    mov ax,[BUTTON_REG] ; store switch values @ 0x0100
    mov [0x101],ax

;    mov bx,@0x0100     ; load LEDs from 0x0100
;    mov ax,[bx]
;    mov bx,@0x2010
;    mov [bx],ax
    mov ax,[0x0100]     ; load LEDs from 0x0100
    mov [0x2010],ax
    
;    mov bx,@0x0101      ; load all displays from 0x0101
;    mov ax,[bx]
    mov ax,[0x0101]     ; load all displays from 0x0101
    mov bx,@0x2020
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    
    jmp Loop

