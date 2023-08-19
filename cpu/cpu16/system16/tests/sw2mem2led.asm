.org 0xf000

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
      mov   ax,[SWITCH_REG]     ; load switch data to ax
      ;mov   [0x0300],ax         ; store to RAM
      mov   [#0],ax         ; store to RAM
      zero  ax                  ; clear ax
      ;mov   ax,[0x0300]         ; load LEDs from RAM
      mov   ax,[#0]         ; load LEDs from RAM
      mov   [LED_REG],ax
      
      jmp   Loop

