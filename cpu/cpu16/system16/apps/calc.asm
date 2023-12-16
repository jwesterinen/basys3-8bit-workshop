;   calc.asm
;
;   This application implements an RPN calculator.  The keypad is used to enter
;   numbers shown on the display.  As keys are pressed, the corresponding number
;   is appended to the displayed value.  The buttons are used to perform 5 functions:
;    - Reset (middle button)
;    - Enter (top button)
;    - Clear entry (bottom button)
;    - Add (left button)
;    - Subtract (right button)
;
;   With an RPN calculator, numbers are entered then the functions are performed.
;   Pressing a function button displays the result of the function.
;
;   Test:
;       1. press "1234"
;       2. press Enter
;       3. press 2345
;       4. press Enter
;       5. press Add
;       6. the value "3579" should be displayed
;
;   Test case 3 -- RPN city
;       Enter      Display
;       2           0002
;       <enter>     0002
;       3           0003
;       <enter>     0003
;       4           0004
;       <enter>     0004
;       5           0005
;       <enter>     0005
;       6           0006
;       <enter>     0006
;       +           000b
;       +           000F
;       +           0012
;       +           0014

#include <system16/system16.asm>
#include <system16/sys.asm>
#include <system16/libasm.asm>

.dz curVal                      ; the current value displayed
.dz buttonCode                  ; the code of the last button pressed
.ds exprStack 20                ; the expression stack is 20 words long


main:
    zero    ep                  ; reset the expression stack pointer
    
Init:
    zero    ax                  ; init current value to "0000"
    mov     [#curVal],ax
    mov     dx,@0xffff          ; set new entry flag to true
    
MainLoop:
    mov     ax,[#curVal]        ; display the current value
    push    ax
    jsr     _Display
    ;pop     ax    

CheckKeys:
    ;push    ax                  ; attempt to modify the current value
    push    dx
    jsr     _AppendKeyValue
    pop     dx                  ; clear the args from the stack
    pop     bx
    mov     cx,ax               ; new value is in cx and ax
    mov     bx,[#curVal]        ; current value is in bx
    sub     cx,bx               ; if the return value is different than the original value 
    bz      CheckButtons 
    mov     [#curVal],ax        ; update the new value 
    zero    dx                  ; and clear the new entry flag

CheckButtons: 
    jsr     _ReadButton         ; check for a button 
    mov     [#buttonCode],ax
    or      ax,@0
    bz      MainLoop
    
DecodeButtons:
    sub     ax,#BUTTON_D        ; Clear button clears the current value
    bz      Init                ; so re-init the calculator
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_U        ; Enter button, goto Enter routine
    bz      Enter
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_L        ; Add button, goto Add routine
    bz      Add
    mov     ax,[#buttonCode]
    sub     ax,#BUTTON_R        ; Subtract button, goto Subract routine
    bz      Subtract
    bra     MainLoop

Enter:
    mov     ax,[#curVal]        ; push curVal onto the eval stack
    jsr     _ExprPush
    mov     dx,#1               ; set new entry flag
    bra     MainLoop
        
Add:
    jsr     _ExprPop            ; ExprPut(ExprTop() + ExprPop());
    mov     bx,ax
    jsr     _ExprTop
    add     ax,bx
    jsr     _ExprPut
    mov     [#curVal],ax        ; set the sum as the current value
    mov     dx,#1               ; set new entry flag
    bra     MainLoop
    
Subtract:
    jsr     _ExprPop            ; ExprPut(ExprTop() - ExprPop());    
    mov     bx,ax
    jsr     _ExprTop
    sub     ax,bx
    jsr     _ExprPut
    mov     [#curVal],ax        ; set the difference as the current value
    mov     dx,#1               ; set new entry flag
    bra     MainLoop

; push the value in ax onto the expression stack    
_ExprPush:
    push    bx
    inc     ep
    mov     bx,@exprStack
    add     bx,ep
    mov     [bx],ax
    pop     bx
    rts
    
; pop the top of the expression stack into ax - if EP is 0, return 0    
_ExprPop:
    zero    ax
    or      ep,#0
    bz      _ExprPop_End
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     ax,[bx]
    dec     ep
    pop     bx
_ExprPop_End:
    rts
    
; copy the top of the expression stack into ax
_ExprTop:
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     ax,[bx]
    pop     bx
    rts

; copy ax into the top of the expression stack    
_ExprPut:
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     [bx],ax
    pop     bx
    rts


