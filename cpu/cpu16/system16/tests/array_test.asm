;   array_test.asm

;   This program tests heap (array) allocation.
;
;   The test passes if "A6" is displayed.

#include <system16/system16.asm>
#include <system16/sys.asm>

.ds array1 20
.ds array2 20

main:
    bsr     Test1
    mov     DISPLAY1_REG,cx     
    bsr     Test2
    mov     DISPLAY2_REG,cx    
     
EndMain:
    bra     EndMain

Test1:
    mov     bx,@array1          ; load the array
    mov     ax,#4
L10:    
    mov     [bx],ax
    inc     bx
    dec     ax
    bpl     L10
    
    mov     bx,@array1          ; sum array elements
    mov     ax,#4
    zero    cx
L11:
    add     cx,[bx]
    inc     bx
    dec     ax
    bpl     L11
    
    rts
    
Test2:
    mov     bx,@array2
    mov     ax,#3
L20:    
    mov     [bx],ax
    inc     bx
    dec     ax
    bpl     L20
    
    mov     bx,@array2
    mov     ax,#3
    zero    cx
L21:
    add     cx,[bx]
    inc     bx
    dec     ax
    bpl     L21
    
    rts
    

