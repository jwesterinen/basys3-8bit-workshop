; complete asm16 syntax test

.org 0xf000

.define Foo 0x12
.define Bar 0x1234

Begin:

Unary:                  ; unary op test
    zero    ax
    loada   bx
    inc     cx
    dec     dx
    asl     ex
    lsr     bp  
    rol     sp  
    ror     ip

DirectReg:
    or      ax,bx
    and     cx,dx
    xor     ex,bp
    mov     ip,sp
    add     ax,ax
    sub     ax,ax
    adc     ax,ax
    sbb     ax,ax
    
IndirectReg:
    or      ax,[bx]
    and     cx,[dx]
    xor     ex,[bp]
    mov     ip,[sp]
    add     bx,[ax]
    sub     dx,[cx]
    adc     bp,[ex]
    sbb     sp,[ip]

Immed8:    
    or      ax,#00
    and     bx,#Foo
    xor     cx,#2
    mov     dx,#04
    mov     dx,#Foo
    add     ex,#0xa
    sub     bp,#20
    adc     sp,#55
    sbb     ip,#0xff

IPRel:
    bra     Begin
    bcc     Immed8
    bcs     CallReg
    bnz     Push
    bz      IPRel
    bpl     Indexed
    bmi     End
    bras    Begin
    bccs    Immed8
    bcss    CallReg
    bnzs    Push
    bzs     IPRel
    bpls    Indexed
    bmis    End

ZP:    
    mov     ax,[#00]
    mov     bx,[#Foo]
    mov     cx,[#2]
    mov     dx,[#04]
    mov     ex,[#0xa]
    mov     bp,[#20]
    mov     sp,[#55]
    mov     ip,[#0xff]
    mov     [#00],ax
    mov     [#Foo],bx
    mov     [#2],cx
    mov     [#04],dx
    mov     [#0xa],ex
    mov     [#20],bp
    mov     [#55],sp
    mov     [#0xff],ip
    
Indexed:
    mov     ax,[ip+00]
    mov     bx,[sp+Foo]
    mov     cx,[bp+2]
    mov     dx,[ex+04]
    mov     ex,[dx+0xa]
    mov     bp,[cx+20]
    mov     sp,[bx+31]
    mov     ip,[ax+0x1f]
    mov     [ip+00],ax
    mov     [sp+Foo],bx
    mov     [bp+2],cx
    mov     [ex+04],dx
    mov     [dx+0xa],ex
    mov     [cx+20],bp
    mov     [bx+31],sp
    mov     [ax+0x1f],ip
    mov     [ip],ax
    mov     [sp],bx
    mov     [bp],cx
    mov     [ex],dx
    mov     [dx],ex
    mov     [cx],bp
    mov     [bx],sp
    mov     [ax],ip

CallReg:
    jsr     ax
    jsr     bx
    jsr     cx
    jsr     dx
    jsr     ex
    jsr     bp  
    jsr     sp  
    jsr     ip
    
RTS:
    rts
    
Push:
    push    ax
    push    bx
    push    cx
    push    dx
    push    ex
    push    bp  
    push    sp  
    push    ip

Pop:
    pop     ax
    pop     bx
    pop     cx
    pop     dx
    pop     ex
    pop     bp  
    pop     sp  
    pop     ip
    
Direct:
    or      ax,@0
    and     bx,@Bar
    xor     cx,@200
    mov     dx,@4004
    mov     dx,@Pop
    mov     dx,@Bar
    add     ex,@0xa
    sub     bp,@0x55
    adc     sp,@0x550
    sbb     ip,@0xffff
    or      ax,0
    and     bx,Bar
    xor     cx,200
    mov     dx,4004
    mov     dx,Bar
    add     ex,0xa
    sub     bp,0x55
    adc     sp,0x550
    sbb     ip,0xffff
    mov     0,ax
    mov     Bar,bx
    mov     200,cx
    mov     4004,dx
    mov     0xa,ex
    mov     0x55,bp
    mov     0x550,sp
    mov     0xffff,ip
    jmp     Pop
    jsr     Push

End:

