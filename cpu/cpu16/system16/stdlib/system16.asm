;   system16.h
;
;   This contains the I/O register definitions for system16.

; general I/O registers
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
.define MOD_SEL_REG         0x3004      ; {0.., noise, VCO2, VCO1}
.define MIXER_SEL_REG       0x3006      ; {0.., LFO, noise, VCO2, VCO1}

; mixer selection values
.define MOD_SEL_VCO1        0x0001
.define MOD_SEL_VCO2        0x0002
.define MOD_SEL_NOISE       0x0004

; mixer selection values
.define MIXER_SEL_VCO1      0x0001
.define MIXER_SEL_VCO2      0x0002
.define MIXER_SEL_NOISE     0x0004
.define MIXER_SEL_LFO       0x0008

; keypad register
.define KEYPAD_REG          0x4000

; button codes
.define BUTTON_NONE         0x00
.define BUTTON_C            0x01
.define BUTTON_U            0x02 
.define BUTTON_L            0x04 
.define BUTTON_R            0x08 
.define BUTTON_D            0x10

; key codes
.define KEY_NONE            0x00
.define KEY_0               0x10
.define KEY_1               0x11
.define KEY_2               0x12
.define KEY_3               0x13
.define KEY_4               0x14     
.define KEY_5               0x15
.define KEY_6               0x16
.define KEY_7               0x17
.define KEY_8               0x18
.define KEY_9               0x19
.define KEY_A               0x1A
.define KEY_B               0x1B
.define KEY_C               0x1C
.define KEY_D               0x1D
.define KEY_E               0x1E
.define KEY_F               0x1F

; display codes
.define DISP_0              0x00
.define DISP_1              0x01
.define DISP_2              0x02
.define DISP_3              0x03
.define DISP_4              0x04
.define DISP_5              0x05
.define DISP_6              0x06
.define DISP_7              0x07
.define DISP_8              0x08
.define DISP_9              0x09
.define DISP_a              0x0a
.define DISP_b              0x0b
.define DISP_c              0x0c
.define DISP_d              0x0d
.define DISP_e              0x0e
.define DISP_f              0x0f
.define DISP_BLANK          0x10
.define DISP_DASH           0x11
.define DISP_J              0x12

