; basic subroutine test

Begin:
    mov     sp,@0x0fff
    jsr     Sub
    
End:
    bra     End
            
Sub:
    rts
    

