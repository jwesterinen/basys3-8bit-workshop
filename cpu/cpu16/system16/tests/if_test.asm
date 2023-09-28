/*  if_test.asm
 *  
 *  This program tests the various condition codes of the cpu16.
 *
 */ 

#include "../system16.h"

; c = 0;
; a = switch & 0x000f;
; b = (switch & 0x00f0) >> 4;
; if (a == b) c |= 0x0001;
; if (a != b) c |= 0x0002;
; if (a < b)  c |= 0x0004;
; if (a >= b) c |= 0x0008;
; if (a > b)  c |= 0x0010;
; print c;

Loop:
    zero    cx                  ; clear LEDs
    mov     LED_REG,cx

    mov     ax,SWITCH_REG       ; ax = switch LSN
    mov     bx,ax               ; bx = switch MSN
    and     ax,@0x000f
    mov     DISPLAY2_REG,ax
    and     bx,@0x00f0
    lsr     bx
    lsr     bx
    lsr     bx
    lsr     bx
    mov     DISPLAY1_REG,bx

TestEq:        
    mov     dx,ax    
    sub     dx,bx               ; set the CCs
    mov     DISPLAY4_REG,dx
    bz      Eq                  ; if (a == b) c |= 0x01;
    ;jmp     TestNe
    bra     TestNe
Eq:
    or      cx,#0x01
    
TestNe:    
    mov     dx,ax    
    sub     dx,bx               ; set the CCs
    bnz     Ne                  ; if (a != b) c |= 0x02;
    jmp     TestLt
Ne:
    or      cx,#0x02

TestLt:    
    mov     dx,ax    
    sub     dx,bx               ; set the CCs
    bmi     Lt                  ; if (a < b) c |= 0x04;
    jmp     TestGe
Lt:
    or      cx,#0x04

TestGe:    
    mov     dx,ax    
    sub     dx,bx               ; set the CCs
    bpl     Ge                  ; if (a >= b) c |= 0x08;
    jmp     TestGt
Ge:
    or      cx,#0x08
    
TestGt:    
    mov     dx,ax    
    sub     dx,bx               ; set the CCs
    bz      End
    bpl     Gt                  ; if (a > b) c |= 0x08;
    jmp     End
Gt:
    or      cx,#0x10
    
End:
    mov     LED_REG,cx          ; print c
    jmp     Loop

