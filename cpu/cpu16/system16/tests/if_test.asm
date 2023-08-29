; logic test

; general I/O registers
.define SWITCH_REG          0x2000
.define BUTTON_REG          0x2002
.define LED_REG             0x2010
.define DISPLAY1_REG        0x2020
.define DISPLAY2_REG        0x2021
.define DISPLAY3_REG        0x2022
.define DISPLAY4_REG        0x2023
.define DISPLAY_CTRL_REG    0x2024

; c = 0;
; ax = switch & 0x000f;
; bx = (switch & 0x00f0) >> 4;
; if (a == b) c |= 0x0001;
; if (a != b) c |= 0x0002;
; if (a < b)  c |= 0x0004;
; if (a >= b) c |= 0x0008;
; if (a > b)  c |= 0x0010;
; print c;

Loop:
    zero    cx                  ; clear LEDs
    mov     [LED_REG],cx

    mov     ax,[SWITCH_REG]     ; ax = switch LSN
    mov     bx,ax               ; bx = switch MSN
    and     ax,@$000f
    mov     [DISPLAY2_REG],ax
    and     bx,@$00f0
    lsr     bx
    lsr     bx
    lsr     bx
    lsr     bx
    mov     [DISPLAY1_REG],bx

TestEq:        
    mov     dx,ax    
    sub     dx,bx               ; set the cc's
    mov     [DISPLAY4_REG],dx
    bz      Eq                  ; if (a == b) c |= 0x01;
    ;jmp     TestNe
    bra     TestNe
Eq:
    or      cx,#$01
    
TestNe:    
    mov     dx,ax    
    sub     dx,bx               ; set the cc's
    bnz     Ne                  ; if (a != b) c |= 0x02;
    jmp     TestLt
Ne:
    or      cx,#$02

TestLt:    
    mov     dx,ax    
    sub     dx,bx               ; set the cc's
    bmi     Lt                  ; if (a < b) c |= 0x04;
    jmp     TestGe
Lt:
    or      cx,#$04

TestGe:    
    mov     dx,ax    
    sub     dx,bx               ; set the cc's
    bpl     Ge                  ; if (a >= b) c |= 0x08;
    jmp     TestGt
Ge:
    or      cx,#$08
    
TestGt:    
    mov     dx,ax    
    sub     dx,bx               ; set the cc's
    bz      End
    bpl     Gt                  ; if (a > b) c |= 0x08;
    jmp     End
Gt:
    or      cx,#$10
    
End:
    mov     [LED_REG],cx        ; print c
    jmp     Loop

