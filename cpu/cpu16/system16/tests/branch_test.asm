;   branch_test.asm
; 
;   This program tests the various conditional branch instructions of the cpu16:
;    - bra
;    - bcc
;    - bcs
;    - bnz
;    - bz
;    - bpl
;    - bmi
;
;   Pseudo code:
;       c = 0;
;       a = b = 1;
;       if (a == b) c |= 0x0001;
;       b = 2;
;       if (a != b) c |= 0x0002;
;       if (a < b)  c |= 0x0004;
;       a = 2;
;       if (a >= b) c |= 0x0008;
;       a = 3;
;       if (a > b)  c |= 0x0010;
;       a = 0xaa + 0x55;
;       if (carry clear) c |= 0x0020;
;       a = 0xaa + 0x56;
;       if (carry set) c |= 0x0040;
;       print c;
;
;   The test passes if the least significant 8 LEDs are turned on.


#include <asm16/system16.asm>

main:
    zero    cx
    
TestEq:
    mov     ax,#1        
    mov     bx,ax    
    sub     ax,bx               ; set the CCs
    bz      Eq                  ; if (a == b) c |= 0x01;
    bra     TestNe
Eq:
    or      cx,#0x01
    
TestNe: 
    mov     ax,#1   
    mov     bx,#2    
    sub     ax,bx               ; set the CCs
    bnz     Ne                  ; if (a != b) c |= 0x02;
    jmp     TestLt
Ne:
    or      cx,#0x02

TestLt:
    mov     ax,#1    
    sub     ax,bx               ; set the CCs
    bmi     Lt                  ; if (a < b) c |= 0x04;
    jmp     TestGe
Lt:
    or      cx,#0x04

TestGe:    
    mov     ax,#2
    sub     ax,bx               ; set the CCs
    bpl     Ge                  ; if (a >= b) c |= 0x08;
    jmp     TestGt
Ge:
    or      cx,#0x08
    
TestGt:    
    mov     ax,#3    
    sub     ax,bx               ; set the CCs
    bz      End
    bpl     Gt                  ; if (a > b) c |= 0x10;
    jmp     TestCc
Gt:
    or      cx,#0x10
    
TestCc:    
    mov     ax,@0xaaaa
    mov     bx,@0x5555
    add     ax,bx               ; set the CCs
    bcc     Cc                  ; if (carry clear) c |= 0x20;
    jmp     TestCs
Cc:
    or      cx,#0x20
    
TestCs:    
    mov     ax,@0xaaaa
    mov     bx,@0x5556
    add     ax,bx               ; set the CCs
    bcs     Cs                  ; if (carry set) c |= 0x40;
    jmp     TestForZero
Cs:
    or      cx,#0x40

TestForZero:    
    mov     ax,@1
    dec     ax                  ; dec op causes the zero bit to be set?????
    bz      EndTestForZero      ; if (!x) c |= 0x80;
    jmp     Print
EndTestForZero:
    or      cx,#0x80

Print:    
    mov     LED_REG,cx          ; print c
    
End:
    bra     End

