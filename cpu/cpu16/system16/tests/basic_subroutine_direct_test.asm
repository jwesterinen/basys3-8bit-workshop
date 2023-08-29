; basic subroutine test

Begin:
    mov     sp,@$0fff
    bra     Main
            
Sub:
    rts
    
Main:    
    ;jsr    Sub
    bsr     Sub
    
End:
    bra     End

