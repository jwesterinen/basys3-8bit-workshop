; basic subroutine test

Begin:
    mov     sp,@$0fff
    
    mov     cx,@Sub
    jsr     cx
    
End:
    jmp     End
            
Sub:
    rts

