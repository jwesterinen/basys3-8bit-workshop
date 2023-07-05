.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2008
      mov   ax,#0xff
      mov   [bx],ax
End:      
      jmp   End

