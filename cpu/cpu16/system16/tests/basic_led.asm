.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2001
      mov   ax,@0x5050
      mov   [bx],ax
      jmp   Loop

