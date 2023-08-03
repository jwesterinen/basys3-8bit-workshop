.arch femto16
.org 0xf000

Loop:
      mov   bx,@0x2002  ; move button state to LEDs
      mov   ax,[bx]
      mov   bx,@0x2010
      mov   [bx],ax
      jmp   Loop

