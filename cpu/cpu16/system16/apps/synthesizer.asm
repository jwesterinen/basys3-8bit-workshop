; Synthesizer

; This is a system16 application that implements a synthesizer by writing to the 
; sound I/O peripheral of the system.  It allows the control of the 2 VCOs and 
; LFO frequencies, which ocillators will be modulated, and which oscillators 
; will be mixed into the output signal.  The interface is as follows:
;   sw[2:0]   controls the frequency of VCO1
;   sw[5:3]   controls the frequency of VCO2
;   sw[8:6]   controls the frequency of the LFO
;   sw[11:9]  controls the modulation selection {noise, VCO2, VCO1}
;   sw[15:12] controls the mixer: {LFO, noise, VCO2, VCO1}
; Note that the noise frequency and LFO depth are hard coded.
;
; The values of each selectable frequency is shown on the display, as follows:
; VCO1 is on display 4, VC02 is on display 3, LFO freq is on display 2,
; LFO selection is on display 1, and the mixer selection is shown on the 
; high nibble of the LEDs.

#include "../system16.asm"

; variables
.define swVal            0x10

.org 0xf000

Begin:

    mov     ax,#90              ; hard code noise = 90
    mov     NOISE_REG,ax
    
    mov     ax,#2               ; hard code LFO depth = @0x0002
    mov     MOD_SEL_REG,ax
    
Loop:
    mov     ax,SWITCH_REG     ; cache the switches
    mov     [#swVal],ax
    
    and     ax,@0x0007          ; VCO1 = (sw & 0x0007) << 6
    mov     DISPLAY4_REG,ax   ; display the VCO1 switches on display 4
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     VCO1_REG,ax

    mov     ax,[#swVal]         ; VCO2 = ((sw & 0x0038) >> 3) << 6
    and     ax,@0x0038
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY3_REG,ax   ; display the VCO2 switches on display 3
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     VCO2_REG,ax

    mov     ax,[#swVal]         ; LFO = ((sw & 0x01c0) >> 6) << 7
    and     ax,@0x01c0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY2_REG,ax   ; display the LFO switches on display 2
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     LFO_REG,ax
    
    mov     ax,[#swVal]         ; modulation = (sw & 0x0e00) >> 9
    and     ax,@0x0e00
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY1_REG,ax   ; display the modulation selection switches on display 1
    mov     LFO_MOD_REG,ax
    
    mov     ax,[#swVal]         ; mixer = (sw & 0xf000) >> 12
    and     ax,@0xf000
    mov     LED_REG,ax        ; display the mixer selection switches on the high nibble of the LEDs
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     MIXER_SEL_REG,ax

    bra     Loop

