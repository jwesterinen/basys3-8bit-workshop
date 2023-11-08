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

#include <asm16/system16.asm>
#include <asm16/sys.asm>
#include <asm16/libasm.asm>

.dz curVal                      ; the current value displayed
.dz buttonCode                  ; the code of the last button pressed

Main:
    zero    ep                  ; reset the expression stack pointer
    jsr     _Beep_Init          ; init the beep
    
Init:
    zero    ax                  ; init current value to "0000"
    mov     [#curVal],ax
    mov     dx,#1               ; set new entry flag
    
MainLoop:
    mov     ax,[#curVal]        ; display the current value
    push    ax
    jsr     _Display
    pop     ax    

CheckKeys:
    jsr     _AppendKeyValue     ; attempt to modify the current value
    mov     [#curVal],ax        ; store the modified value

CheckButtons:    
    jsr     _ReadButton         ; store the next button pressed
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

