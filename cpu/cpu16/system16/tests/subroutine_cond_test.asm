; subroutine_cond_test
;
;   This tests conditional subroutine calls.
;
;   Pseudocode
;       while (1)
;       {
;           op1 = GetSwitches() & 0x00f0 >> 4;
;           display(op1, 1);
;           op2 = GetSwitches() & 0x000f;
;           display(op2, 2);
;
;           if (GetSwitches() & 0x8000)
;               val = Add(op1, op2);
;           else
;               val = Sub(op1, op2);
;           display(val, 4);
;       }
;
;   Test procedure:
;    1. Choose a 4-bit number using switches 3-0.
;    2. Choose a 4-bit number using switches 7-4.
;    3. If switch 15 is 0, the sum should be displayed.
;    4. If switch 15 is 1, the difference should be displayed.
;

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"

Add:
    add     ax,bx
    rts
    
Main:
    mov     ax,SWITCH_REG       ; ax = switch[7:4]
    mov     bx,ax               ; bx = switch[3:0]
    mov     cx,ax               ; cx = switch[15]
    and     ax,#0xf0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY1_REG,ax
    and     bx,#0x0f
    mov     DISPLAY2_REG,bx
    and     cx,@0x8000
    
    sub     cx,#0
    bz      L1
    bras    Sub                 ; if switch != 0, call Sub via branch with params in ax and bx
    bra     L2
L1:
    bras    Add                 ; if switch == 0, call Add via branch with params in ax and bx
L2:
            
EndMain:
    mov     DISPLAY4_REG,ax   ; display results
    jmp     Main
    
Sub:
    sub     ax,bx
    rts
    

