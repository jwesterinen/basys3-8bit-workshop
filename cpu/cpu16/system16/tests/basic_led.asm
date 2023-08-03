.arch femto16
.org 0xf000

Loop:
      mov   bx,@0x2010
      mov   ax,@0x5050
      mov   [bx],ax
      jmp   Loop

