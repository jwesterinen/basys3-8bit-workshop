.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2000
      mov   ax,[bx]
      mov   bx,@0x2004
      mov   [bx],ax
      
      mov   bx,@0x2001
      mov   ax,[bx]
      mov   bx,@0x2005
      mov   [bx],ax

      mov   bx,@0x2002
      mov   ax,[bx]
      mov   bx,@0x2006
      mov   [bx],ax

      jmp   Loop

