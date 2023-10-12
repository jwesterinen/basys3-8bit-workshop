;   branch_test.asm
; 
;   This program tests the various conditional branch instructions of the cpu16:
;    - bras
;    - bccs
;    - bcss
;    - bnzs
;    - bzs
;    - bpls
;    - bmis
;
;   Pseudo code:
;       Eq(){c |= 0x0001;}
;       Ne(){c |= 0x0002;}
;       Lt(){c |= 0x0004;}
;
;       main()
;       {
;           int a, b, c;
;
;           c = 0;
;
;           a = b = 1;
;           if (a == b) Eq();
;           b = 2;
;           if (a != b) Ne();
;           if (a < b)  Lt();
;           a = 2;
;           if (a >= b) Ge();
;           a = 3;
;           if (a > b)  Gt();
;           a = 0xaa + 0x55;
;           if (carry clear) Cc();
;           a = 0xaa + 0x56;
;           if (carry set) Cs();
;
;           print c;
;       }
;
;       Ge(){c |= 0x0008;}
;       Gt(){c |= 0x0010;}
;       Cc(){c |= 0x0020;}
;       Cs(){c |= 0x0040;}
;
;   The test passes if the least significant 7 LEDs are turned on.

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"

; subroutines IP relative negative
    
Eq:
    or      cx,#0x0001
    rts
    
Ne:
    or      cx,#0x0002
    rts
    
Lt:
    or      cx,#0x0004
    rts

    
Main:
    zero    cx
    
TestEq:
    mov     ax,#1        
    mov     bx,ax    
    sub     ax,bx               ; set the CCs
    bzs     Eq                  ; if (a == b) Eq();
    
TestNe: 
    mov     ax,#1   
    mov     bx,#2    
    sub     ax,bx               ; set the CCs
    bnzs    Ne                  ; if (a != b) Ne();

TestLt:
    mov     ax,#1    
    sub     ax,bx               ; set the CCs
    bmis    Lt                  ; if (a < b) Lt();

TestGe:    
    mov     ax,#2
    sub     ax,bx               ; set the CCs
    bpls    Ge                  ; if (a >= b) Ge();
    
TestGt:    
    mov     ax,#3    
    sub     ax,bx               ; set the CCs
    bpls    Gt                  ; if (a > b) Gt();
    
TestCc:    
    mov     ax,@0xaaaa
    mov     bx,@0x5555
    add     ax,bx               ; set the CCs
    bccs    Cc                  ; if (carry clear) Cc();
    
TestCs:    
    mov     ax,@0xaaaa
    mov     bx,@0x5556
    add     ax,bx               ; set the CCs
    bcss    Cs                  ; if (carry set) Cs();

Print:    
    mov     LED_REG,cx          ; print c
    
End:
    bra     End


; subroutines IP relative positive
    
Ge:
    or      cx,#0x0008
    rts
    
Gt:
    or      cx,#0x0010
    rts
    
Cc:
    or      cx,#0x0020
    rts
    
Cs:
    or      cx,#0x0040
    rts

