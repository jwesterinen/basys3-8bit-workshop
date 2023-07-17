.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2001
      mov   ax,[bx]
      mov   bx,@0x2002
      mov   [bx],ax
      jmp   Loop

