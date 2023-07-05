.arch femto16
.org 0xf000
.len 4096
Loop:
      mov   bx,@0x2000      ; load switch data to ax
      mov   ax,[bx]
      mov   bx,@0x0300      ; store to RAM
      mov   [bx],ax
      zero  ax              ; clear ax
      mov   ax,[bx]         ; load LEDs from RAM
      mov   bx,@0x2001
      mov   [bx],ax
      jmp   Loop

