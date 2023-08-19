.org 0xf000

.define SWITCH_REG 0x2000
.define LED_REG 0x2010

Loop:
      mov   bx,@SWITCH_REG
      mov   ax,[bx]
      mov   bx,@LED_REG
      mov   [bx],ax
      jmp   Loop

