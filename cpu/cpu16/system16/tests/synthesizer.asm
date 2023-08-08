; Synthesizer

; This is a system16 application that implements a synthesizer by writing the 
; sound I/O peripheral of the system.  It allows the control of the 2 VCOs and 
; LFO frequencies, which ocillators will be modulated, and which oscillators 
; will be mixed into the output signal.  The interface is as follows:
;   sw[2:0]   controls the frequency of VCO1
;   sw[5:3]   controls the frequency of VCO2
;   sw[8:6]   controls the frequency of the LFO
;   sw[11:9]  controls the modulation selection {noise, VCO2, VCO1}
;   sw[15:12] controls the mixer: {LFO, noise, VCO2, VCO1}
; Note that the noise frequency and LFO depth are hard coded.

.org 0xf000

Begin:

    mov     ax,#90      ; hard code noise = 90
    mov     bx,@0x3002
    mov     [bx],ax

    
    mov     ax,@0x0002  ; hard code LFO depth = @0x0002
    mov     bx,@0x3004
    mov     [bx],ax
    
Loop:
    mov     bx,@0x2000  ; VCO1 = (sw & 0x0007) << 6
    mov     ax,[bx]
    and     ax,@0x0007
    mov     bx,@0x2023  ; display the VCO1 switches on display 4
    mov     [bx],ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     bx,@0x3000
    mov     [bx],ax

    mov     bx,@0x2000  ; VCO2 = ((sw & 0x0038) >> 3) << 6
    mov     ax,[bx]
    and     ax,@0x0038
    lsr     ax
    lsr     ax
    lsr     ax
    mov     bx,@0x2022  ; display the VCO2 switches on display 3
    mov     [bx],ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     bx,@0x3001
    mov     [bx],ax

    mov     bx,@0x2000  ; LFO = ((sw & 0x01c0) >> 6) << 7
    mov     ax,[bx]
    and     ax,@0x01c0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     bx,@0x2021  ; display the LFO switches on display 2
    mov     [bx],ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    asl     ax
    mov     bx,@0x3003
    mov     [bx],ax
    
    mov     bx,@0x2000  ; modulation = (sw & 0x0e00) >> 9
    mov     ax,[bx]
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
    mov     bx,@0x2020  ; display the modulation selection switches on display 1
    mov     [bx],ax
    mov     bx,@0x3005
    mov     [bx],ax
    
    mov     bx,@0x2000  ; mixer = (sw & 0xf000) >> 12
    mov     ax,[bx]
    and     ax,@0xf000
    mov     bx,@0x2010  ; display the mixer selection switches on the high nibble of the LEDs
    mov     [bx],ax
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
    mov     bx,@0x3006
    mov     [bx],ax

    jmp     Loop

