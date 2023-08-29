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

Loop:
    mov   bx,@SWITCH_REG
    mov   ax,[bx]
    mov   bx,@LED_REG
    mov   [bx],ax
    
    bra Loop

