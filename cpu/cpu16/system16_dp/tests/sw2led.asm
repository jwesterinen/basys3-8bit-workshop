.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2000
      mov   ax,[bx]
      mov   bx,@0x2008
      mov   [bx],ax
      jmp   Loop
