
; begin program
; initialize the runtime
    mov     sp,@0x0fff
    zero    bp
    jmp     main

; define all register definitions and return codes
#include <asm16/system16.asm>

; insert all libasm code
#include <asm16/libasm.asm>


; fct entry
foo3:
    push    bp
    mov     bp,sp
    sub     sp,#_LP1

; load immed
    mov     ax,@3	; constant
    push    ax

; load indirect
    mov     bx,bp	; param n[]
    add     bx,#3
    mov     bx,[bx]
    pop     ax
    sub     bx,ax
    push    bx

; load immed
    mov     ax,@3	; constant
    push    ax

; store indirect
    pop     ax
    pop     bx
    mov     [bx],ax

; return
    mov     sp,bp
    pop     bp
    rts
.define _LP1 0

; fct entry
foo4:
    push    bp
    mov     bp,sp
    sub     sp,#_LP2

; load immed
    mov     ax,@4	; constant
    push    ax

; load indirect
    mov     bx,bp	; param n[]
    add     bx,#3
    mov     bx,[bx]
    pop     ax
    sub     bx,ax
    push    bx

; load immed
    mov     ax,@4	; constant
    push    ax

; store indirect
    pop     ax
    pop     bx
    mov     [bx],ax

; return
    mov     sp,bp
    pop     bp
    rts
.define _LP2 0

; fct entry
main:
    push    bp
    mov     bp,sp
    sub     sp,#_LP3

; load immed
    mov     ax,@0x2010	; constant
    push    ax

; store direct
    mov     bx,bp	; local pLeds
    sub     bx,#6
    pop     ax
    mov     [bx],ax

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@0	; constant
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax

; load immed
    mov     ax,@0x2020	; constant
    push    ax

; store direct
    mov     bx,#0x1f	; global pD1
    pop     ax
    mov     [bx],ax

; load immed
    mov     ax,@0x2021	; constant
    push    ax

; store direct
    mov     bx,#0x20	; global pD2
    pop     ax
    mov     [bx],ax

; load immed
    mov     ax,@0x2022	; constant
    push    ax

; store direct
    mov     bx,#0x21	; global pD3
    pop     ax
    mov     [bx],ax

; load immed
    mov     ax,@0x2023	; constant
    push    ax

; store direct
    mov     bx,#0x22	; global pD4
    pop     ax
    mov     [bx],ax

; load immed
    mov     ax,@1	; constant
    push    ax

; load indirect
    mov     bx,bp	; local a[]
    sub     bx,#0
    pop     ax
    sub     bx,ax
    push    bx

; load immed
    mov     ax,@1	; constant
    push    ax

; store indirect
    pop     ax
    pop     bx
    mov     [bx],ax

; load immed
    mov     ax,@1	; constant
    push    ax

; load indirect
    mov     bx,bp	; local a[]
    sub     bx,#0
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store direct
    mov     bx,bp	; local x
    sub     bx,#5
    pop     ax
    mov     [bx],ax

; load direct
    mov     bx,bp	; local x
    sub     bx,#5
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@1	; constant
    push    ax

; alu, ==, ==
    mov     cx,@0xffff
    pop     bx
    pop     ax
    sub     ax,bx
    bz      LT1
    mov     cx,#0
LT1:
    push    cx

; jumpz	IF
    pop     ax
    or      ax,#0
    bz      _LP4

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     ax,[bx]
    push    ax

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     bx,[bx]
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@0x0008	; constant
    push    ax

; alu, |, |
    pop    bx
    pop    ax
    or     ax,bx
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax
_LP4:

; load pointer
    mov     bx,#0x1f	; global *pD1
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@1	; constant
    push    ax

; load indirect
    mov     bx,bp	; local a[]
    sub     bx,#0
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax

; load immed
    mov     ax,@2	; constant
    push    ax

; load indirect
    mov     bx,#0x1e	; global b[]
    pop     ax
    sub     bx,ax
    push    bx

; load immed
    mov     ax,@2	; constant
    push    ax

; store indirect
    pop     ax
    pop     bx
    mov     [bx],ax

; load immed
    mov     ax,@2	; constant
    push    ax

; load indirect
    mov     bx,#0x1e	; global b[]
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store direct
    mov     bx,bp	; local x
    sub     bx,#5
    pop     ax
    mov     [bx],ax

; load direct
    mov     bx,bp	; local x
    sub     bx,#5
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@2	; constant
    push    ax

; alu, ==, ==
    mov     cx,@0xffff
    pop     bx
    pop     ax
    sub     ax,bx
    bz      LT2
    mov     cx,#0
LT2:
    push    cx

; jumpz	IF
    pop     ax
    or      ax,#0
    bz      _LP5

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     ax,[bx]
    push    ax

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     bx,[bx]
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@0x0004	; constant
    push    ax

; alu, |, |
    pop    bx
    pop    ax
    or     ax,bx
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax
_LP5:

; load pointer
    mov     bx,#0x20	; global *pD2
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@2	; constant
    push    ax

; load indirect
    mov     bx,#0x1e	; global b[]
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax

; load reference
    mov     bx,bp	; local &a
    sub     bx,#0
    push    bx

; call
    jsr     foo3
    pop     bx ; discard argument

; load immed
    mov     ax,@3	; constant
    push    ax

; load indirect
    mov     bx,bp	; local a[]
    sub     bx,#0
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store direct
    mov     bx,bp	; local x
    sub     bx,#5
    pop     ax
    mov     [bx],ax

; load direct
    mov     bx,bp	; local x
    sub     bx,#5
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@3	; constant
    push    ax

; alu, ==, ==
    mov     cx,@0xffff
    pop     bx
    pop     ax
    sub     ax,bx
    bz      LT3
    mov     cx,#0
LT3:
    push    cx

; jumpz	IF
    pop     ax
    or      ax,#0
    bz      _LP6

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     ax,[bx]
    push    ax

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     bx,[bx]
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@0x0002	; constant
    push    ax

; alu, |, |
    pop    bx
    pop    ax
    or     ax,bx
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax
_LP6:

; load pointer
    mov     bx,#0x21	; global *pD3
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@3	; constant
    push    ax

; load indirect
    mov     bx,bp	; local a[]
    sub     bx,#0
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax

; load reference
    mov     bx,#0x1e	; global &b
    push    bx

; call
    jsr     foo4
    pop     bx ; discard argument

; load immed
    mov     ax,@4	; constant
    push    ax

; load indirect
    mov     bx,#0x1e	; global b[]
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store direct
    mov     bx,bp	; local x
    sub     bx,#5
    pop     ax
    mov     [bx],ax

; load direct
    mov     bx,bp	; local x
    sub     bx,#5
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@4	; constant
    push    ax

; alu, ==, ==
    mov     cx,@0xffff
    pop     bx
    pop     ax
    sub     ax,bx
    bz      LT4
    mov     cx,#0
LT4:
    push    cx

; jumpz	IF
    pop     ax
    or      ax,#0
    bz      _LP7

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     ax,[bx]
    push    ax

; load pointer
    mov     bx,bp	; local *pLeds
    sub     bx,#6
    mov     bx,[bx]
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@0x0001	; constant
    push    ax

; alu, |, |
    pop    bx
    pop    ax
    or     ax,bx
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax
_LP7:

; load pointer
    mov     bx,#0x22	; global *pD4
    mov     ax,[bx]
    push    ax

; load immed
    mov     ax,@4	; constant
    push    ax

; load indirect
    mov     bx,#0x1e	; global b[]
    pop     ax
    sub     bx,ax
    mov     ax,[bx]
    push    ax

; store pointer
    pop    ax
    pop    bx
    mov    [bx],ax

; return
    jmp    __Exit
.define _LP3 7

; end program
__Exit:
    bra    __Exit

